/* vim:set noet ts=8 sw=8 : */

#define GREENLET_MODULE
#include "greenlet.h"

#include <assert.h>
#include <limits.h>
#include <memory.h>
#include <stdlib.h>


#define PyThreadState gr_thread
#define PyTypeObject void*
#define PyObject void	

#define PyThreadState_GetDict() _thread.dict
#define PyThreadState_GET() (&_thread)

#define PyGreenlet_STARTED(op)    (((greenlet*)(op))->stack_stop != NULL)
#define PyGreenlet_ACTIVE(op)     (((greenlet*)(op))->stack_start != NULL)
#define PyGreenlet_GET_PARENT(op) (((greenlet*)(op))->parent)



/***********************************************************

A PyGreenlet is a range of C stack addresses that must be
saved and restored in such a way that the full range of the
stack contains valid data when we switch to it.

Stack layout for a greenlet:

               |     ^^^       |
               |  older data   |
               |               |
  stack_stop . |_______________|
        .      |               |
        .      | greenlet data |
        .      |   in stack    |
        .    * |_______________| . .  _____________  stack_copy + stack_saved
        .      |               |     |             |
        .      |     data      |     |greenlet data|
        .      |   unrelated   |     |    saved    |
        .      |      to       |     |   in heap   |
 stack_start . |     this      | . . |_____________| stack_copy
               |   greenlet    |
               |               |
               |  newer data   |
               |     vvv       |


Note that a greenlet's stack data is typically partly at its correct
place in the stack, and partly saved away in the heap, but always in
the above configuration: two blocks, the more recent one in the heap
and the older one still in the stack (either block may be empty).

Greenlets are chained: each points to the previous greenlet, which is
the one that owns the data currently in the C stack above my
stack_stop.  The currently running greenlet is the first element of
this chain.  The main (initial) greenlet is the last one.  Greenlets
whose stack is entirely in the heap can be skipped from the chain.

The chain is not related to execution order, but only to the order
in which bits of C stack happen to belong to greenlets at a particular
point in time.

The main greenlet doesn't have a stack_stop: it is responsible for the
complete rest of the C stack, and we don't know where it begins.  We
use (char*) -1, the largest possible address.

States:
  stack_stop == NULL && stack_start == NULL:  did not start yet
  stack_stop != NULL && stack_start == NULL:  already finished
  stack_stop != NULL && stack_start != NULL:  active

The running greenlet's stack_start is undefined but not NULL.

 ***********************************************************/

/*** global state ***/

/* In the presence of multithreading, this is a bit tricky:

   - ts_current always store a reference to a greenlet, but it is
     not really the current greenlet after a thread switch occurred.

   - each *running* greenlet uses its run_info field to know which
     thread it is attached to.  A greenlet can only run in the thread
     where it was created.  This run_info is a ref to tstate->dict.

   - the thread state dict is used to save and restore ts_current,
     using the dictionary key 'ts_curkey'.
*/


/* The current greenlet in this thread state (holds a reference) */
static greenlet* volatile ts_current = NULL;
/* Holds a reference to the switching-to stack during the slp switch */
static greenlet* volatile ts_target = NULL;

/***********************************************************/
/* Thread-aware routines, switching global variables when needed */

#define STATE_OK    (1 \
			|| !green_updatecurrent())




static greenlet* green_create_main(void)
{
	greenlet* gmain = (greenlet*)malloc(sizeof(greenlet)); 
	if (gmain == NULL)
		return NULL;
	memset(gmain, 0, sizeof(greenlet));
	gmain->stack_start = (char*) 1;
	gmain->stack_stop = (char*) -1;

	return gmain;
}

static int green_updatecurrent(void)
{
	if (ts_current == 0)
	{
		/* first time we see this tstate */
		ts_current = green_create_main();
	}
	return 0;
}


/***********************************************************/

/* Some functions must not be inlined:
   * slp_restore_state, when inlined into slp_switch might cause
     it to restore stack over its own local variables
   * slp_save_state, when inlined would add its own local
     variables to the saved stack, wasting space
   * slp_switch, cannot be inlined for obvious reasons
   * g_initialstub, when inlined would receive a pointer into its
     own stack frame, leading to incomplete stack save/restore
*/

#if defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
#define GREENLET_NOINLINE_SUPPORTED
#define GREENLET_NOINLINE(name) __attribute__((noinline)) name
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
#define GREENLET_NOINLINE_SUPPORTED
#define GREENLET_NOINLINE(name) __declspec(noinline) name
#endif

#ifdef GREENLET_NOINLINE_SUPPORTED
/* add forward declarations */
static void GREENLET_NOINLINE(slp_restore_state)(void);
static int GREENLET_NOINLINE(slp_save_state)(char*);
#if !(defined(MS_WIN64) && defined(_M_X64))
static int GREENLET_NOINLINE(slp_switch)(void);
#endif
static void GREENLET_NOINLINE(g_initialstub)(void*);
#define GREENLET_NOINLINE_INIT() do {} while(0)
#else
/* force compiler to call functions via pointers */
static void (*slp_restore_state)(void);
static int (*slp_save_state)(char*);
static int (*slp_switch)(void);
static void (*g_initialstub)(void*);
#define GREENLET_NOINLINE(name) cannot_inline_ ## name
#define GREENLET_NOINLINE_INIT() do { \
	slp_restore_state = GREENLET_NOINLINE(slp_restore_state); \
	slp_save_state = GREENLET_NOINLINE(slp_save_state); \
	slp_switch = GREENLET_NOINLINE(slp_switch); \
	g_initialstub = GREENLET_NOINLINE(g_initialstub); \
} while(0)
#endif

/***********************************************************/

static int g_save(greenlet* g, char* stop)
{
	/* Save more of g's stack into the heap -- at least up to 'stop'

	   g->stack_stop |________|
	                 |        |
			 |    __ stop       . . . . .
	                 |        |    ==>  .       .
			 |_______|         _______
			 |       |        |       |
			 |       |        |       |
	  g->stack_start |        |       |_______| g->stack_copy

	 */
	intptr_t sz1 = g->stack_saved;
	intptr_t sz2 = stop - g->stack_start;
	assert(g->stack_start != NULL);
	if (sz2 > sz1) 
	{
		char* c = (char*)realloc(g->stack_copy, sz2);
		if (!c) 
		{
			assert(0);
			return -1;
		}
		memcpy(c+sz1, g->stack_start+sz1, sz2-sz1);
		g->stack_copy = c;
		g->stack_saved = sz2;
	}
	return 0;
}

static void GREENLET_NOINLINE(slp_restore_state)(void)
{
	greenlet* g = ts_target;
	greenlet* owner = ts_current;
	
	/* Restore the heap copy back into the C stack */
	if (g->stack_saved != 0) 
	{
		memcpy(g->stack_start, g->stack_copy, g->stack_saved);
		//PyMem_Free(g->stack_copy);
		free(g->stack_copy);
		g->stack_copy = NULL;
		g->stack_saved = 0;
	}
	if (owner->stack_start == NULL)
		owner = owner->stack_prev; /* greenlet is dying, skip it */
	while (owner && owner->stack_stop <= g->stack_stop)
		owner = owner->stack_prev; /* find greenlet with more stack */
	g->stack_prev = owner;
}

static int GREENLET_NOINLINE(slp_save_state)(char* stackref)
{
	/* must free all the C stack up to target_stop */
	char* target_stop = ts_target->stack_stop;
	greenlet* owner = ts_current;
	assert(owner->stack_saved == 0);
	if (owner->stack_start == NULL)
		owner = owner->stack_prev;  /* not saved if dying */
	else
		owner->stack_start = stackref;
	
	while (owner->stack_stop < target_stop) 
	{
		/* ts_current is entierely within the area to free */
		if (g_save(owner, owner->stack_stop))
			return -1;  /* XXX */
		owner = owner->stack_prev;
	}

	if (owner != ts_target) 
	{
		if (g_save(owner, target_stop))
			return -1;  /* XXX */
	}
	return 0;
}


/*
 * the following macros are spliced into the OS/compiler
 * specific code, in order to simplify maintenance.
 */

#define SLP_SAVE_STATE(stackref, stsizediff)		\
  stackref += STACK_MAGIC;				\
  if (slp_save_state((char*)stackref)) return -1;	\
  if (!PyGreenlet_ACTIVE(ts_target)) return 1;		\
  stsizediff = ts_target->stack_start - (char*)stackref

#define SLP_RESTORE_STATE()			\
  slp_restore_state()


#define SLP_EVAL
#define slp_switch GREENLET_NOINLINE(slp_switch)

#ifdef WIN32
#define MS_WIN32
#endif

#include "slp_platformselect.h"
#undef slp_switch

#ifndef STACK_MAGIC
#error "greenlet needs to be ported to this platform,\
 or teached how to detect your compiler properly."
#endif /* !STACK_MAGIC */

#ifdef EXTERNAL_ASM
/* CCP addition: Make these functions, to be called from assembler.
 * The token include file for the given platform should enable the
 * EXTERNAL_ASM define so that this is included.
 */

intptr_t slp_save_state_asm(intptr_t *ref) {
    intptr_t diff;
    SLP_SAVE_STATE(ref, diff);
    return diff;
}

void slp_restore_state_asm(void) {
    SLP_RESTORE_STATE();
}

extern int slp_switch(void);

#endif

static int g_switchstack(void)
{
	/* perform a stack switch according to some global variables
	   that must be set before:
	   - ts_current: current greenlet (holds a reference)
	   - ts_target: greenlet to switch to
	   - ts_passaround_args: NULL if PyErr_Occurred(),
	             else a tuple of args sent to ts_target (holds a reference)
	   - ts_passaround_kwargs: same as ts_passaround_args
	*/
	int err = slp_switch();
	if (err < 0)
	{
		/* error */
	}
	else 
	{
		greenlet* target = ts_target;
		greenlet* origin = ts_current;
		
		ts_current = target;

	}
	return err;
}

static gr_param
g_switch(greenlet* target, gr_param data)
{
	/* _consumes_ a reference to the args tuple and kwargs dict,
	   and return a new tuple reference */

	/* check ts_current */
	if (!STATE_OK)
		return 0;


	/* find the real target by ignoring dead greenlets,
	   and if necessary starting a greenlet. */
	while (1) 
	{
		if (PyGreenlet_ACTIVE(target)) 
		{
			ts_target = target;
			ts_target->data = data;
			g_switchstack();
			break;
		}

		if (!PyGreenlet_STARTED(target)) 
		{
			void* dummymarker;
			ts_target = target;
			ts_target->data = data;
			g_initialstub(&dummymarker);
			break;
		}
		target = target->parent;
	}
    return ts_current->data;
}

static void GREENLET_NOINLINE(g_initialstub)(void* mark)
{
	int err;

	gr_param ret;

	/* now use run_info to store the statedict */

	
	/* start the greenlet */
	ts_target->stack_start = NULL;
	ts_target->stack_stop = (char*) mark;
	if (ts_current->stack_start == NULL) 
	{
		/* ts_current is dying */
		ts_target->stack_prev = ts_current->stack_prev;
	}
	else 
	{
		ts_target->stack_prev = ts_current;
	}

	err = g_switchstack();
	/* returns twice!
	   The 1st time with err=1: we are in the new greenlet
	   The 2nd time with err=0: back in the caller's greenlet
	*/
	if (err == 1) 
	{
		/* in the new greenlet */

		greenlet* ts_self = ts_current;
		ts_self->stack_start = (char*) 1;

		
		ret = ts_self->callback(ts_self->data);
		//result = g_handle_exit(result);

		/* jump back to parent */
		ts_self->stack_start = NULL;  /* dead */
		g_switch(ts_self->parent, ret);
		assert(0);
		/* must not return from here! */
		//PyErr_WriteUnraisable((PyObject *) ts_self);
		//Py_FatalError("greenlets cannot continue");
	}
	/* back in the parent */
}


/***********************************************************/
greenlet *greenlet_create(gr_callback cb)
{	
	greenlet *gr = (greenlet *)malloc(sizeof(greenlet));
	memset(gr, 0, sizeof(greenlet));

	if (!ts_current)
		ts_current = green_create_main();

	gr->parent = ts_current;
	gr->callback = cb;
	return gr;
}

//static int green_setrun(PyGreenlet* self, PyObject* nparent, void* c);
static int green_setparent(greenlet* self, PyObject* nparent, void* c);

static int kill_greenlet(greenlet* self)
{
	/* Cannot raise an exception to kill the greenlet if
	   it is not running in the same thread! */
	
	/* The dying greenlet cannot be a parent of ts_current
		because the 'parent' field chain would hold a
		reference */
	gr_param result;
	greenlet* oldparent;
	if (!STATE_OK) 
	{
		return -1;
	}
	oldparent = self->parent;
	self->parent = ts_current;
		
	/* Send the greenlet a GreenletExit exception. */
	//PyErr_SetNone(PyExc_GreenletExit);
	result = g_switch(self, 0);
	if (result == 0)
		return -1;
	free(self);
	return 0;
}

void greenlets_init()
{
	//GREENLET_NOINLINE_INIT();

	ts_current = green_create_main();
}

gr_param greenlet_run(greenlet* self, gr_param data)
{
	return greenlet_switch(self, data);
}

gr_param greenlet_switch(greenlet* self, gr_param data)
{

	if (self->stack_stop && !self->stack_start)
	{
		assert(!"finished");
		return 0;
	}
	
	if (!STATE_OK)
		return 0;


	return g_switch(self, data);
}

void greenlet_kill(greenlet *gr)
{

}


int green_setrun(greenlet* self, PyObject* nrun, void* c)
{
	if (PyGreenlet_STARTED(self)) 
	{
		/*
		PyErr_SetString(PyExc_AttributeError,
				"run cannot be set "
				"after the start of the greenlet");
				*/
		assert(0);
		return -1;
	}
	return 0;
}

greenlet* greenlet_getparent()
{
	return ts_current->parent;
}

greenlet *greenlet_getcurrent()
{
	return ts_current;
}


static int green_setparent(greenlet* self, PyObject* nparent, void* c)
{
	greenlet* p;
	if (nparent == NULL) 
	{
		assert(0);
//		PyErr_SetString(PyExc_AttributeError, "can't delete attribute");
		return -1;
	}

	for (p=(greenlet*) nparent; p; p=p->parent) 
	{
		if (p == self) 
		{
			assert(0);
//			PyErr_SetString(PyExc_ValueError, "cyclic parent chain");
			return -1;
		}
	}
	p = self->parent;
	self->parent = (greenlet*) nparent;

	return 0;
}