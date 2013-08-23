/* vim:set noet ts=8 sw=8 : */

/* Greenlet object interface */

#ifndef GREENLETOBJECT_H
#define GREENLETOBJECT_H

#ifdef _MSC_VER
typedef __int32 int32_t;
typedef int32_t intptr_t;
#else
#include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

	
typedef struct _gr_dict  *gr_dict_p;
typedef int gr_param;
typedef gr_param (*gr_callback)(gr_param);

typedef struct _greenlet 
{
	char* stack_start;
	char* stack_stop;
	char* stack_copy;
	intptr_t stack_saved;
	struct _greenlet* stack_prev;
	struct _greenlet* parent;

	gr_callback callback;
	gr_param data;

} greenlet;


void greenlets_init();

greenlet *greenlet_create(gr_callback cb);
greenlet *greenlet_getparent();
greenlet *greenlet_getcurrent();

void	 greenlet_kill(greenlet *);
gr_param greenlet_run(greenlet* self, gr_param data);
gr_param greenlet_switch(greenlet* self, gr_param data);


#ifdef __cplusplus
}
#endif
#endif /* !GREENLETOBJECT_H */
