/*
** ADOBE SYSTEMS INCORPORATED
** Copyright 2012 Adobe Systems Incorporated. All Rights Reserved.
**
** NOTICE:  Adobe permits you to use, modify, and distribute this file in
** accordance with the terms of the Adobe license agreement accompanying it.
** If you have received this file from a source other than Adobe, then your use,
** modification, or distribution of it requires the prior written permission of Adobe.
*/

#include <AS3/AVM2.h>
#include <AS3/AS3.h>
#include <AS3/AS3++.h>

#ifndef AS3WIG_BOILERPLATE
#define AS3WIG_BOILERPLATE

#define AS3_GET_ID(X) (((AS3::local::internal::id_holder *)&(X))->_id)
#define AS3_ARG_SIZE(X) ((sizeof(X) + 3) & ~3)
static inline void *AS3_FPTR_(char c, ...) { __builtin_va_list ap; __builtin_va_start(ap, c); void *r = __builtin_va_arg(ap, void *); __builtin_va_end(ap); return r; }
#define AS3_FPTR(X) AS3_FPTR_(-1, (X))

struct AS3_DELEGATE_REQ
{
  void *proc;
  void *args;
  int size;
  struct __avm2_retdata retdata;
};

static void *AS3_DELEGATE_THUNK(void *data)
{
  AS3_DELEGATE_REQ *req = (AS3_DELEGATE_REQ *)data;
  try
  {
    void *ret = __avm2_apply(req->proc, req->args, req->size);
    memcpy(&req->retdata, ret, sizeof(struct __avm2_retdata));
  }
  catch(AS3::ui::var &v)
  {
    unsigned eid = AS3_GET_ID(v);
    AS3_GET_ID(v) = 0;
    return (void *)eid;
  }
  return (void *)-1;
}

#define AS3_MAYBE_DELEGATE(X, Y) if(AS3_UI && !avm2_is_ui_worker()) { \
  AS3_DELEGATE_REQ req = { AS3_FPTR(X), __avm2_apply_args(), (Y) }; \
  unsigned eid = (unsigned)avm2_ui_thunk(AS3_DELEGATE_THUNK, &req);\
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  __avm2_return(&req.retdata); \
}

#define AS3_BASE_DEF(N, D) \
  N() { _id = AS3_NS::var::internal::D; } \
  N(const N &); \
  N(var) __attribute__((noinline)); \
  N &operator=(const N &) __attribute__((noinline)); \
  N &operator=(var) __attribute__((noinline))

#define AS3_IPROP_SET_DEF(T, N) \
class iprop_##N { \
public: \
  iprop_##N &operator=(T) __attribute__((noinline)); \
}
#define AS3_IPROP_GET_DEF(T, N) \
class iprop_##N { \
public: \
  operator T() __attribute__((noinline)); \
}
#define AS3_IPROP_SET_GET_DEF(T, N) \
class iprop_##N { \
public: \
  operator T() __attribute__((noinline)); \
  iprop_##N &operator=(T) __attribute__((noinline)); \
}

#define AS3_PROP_SET_DEF(T, N) \
class prop_##N { \
public: \
  prop_##N &operator=(T) __attribute__((noinline)); \
}
#define AS3_PROP_GET_DEF(T, N) \
class prop_##N { \
public: \
  operator T() __attribute__((noinline)); \
}
#define AS3_PROP_SET_GET_DEF(T, N) \
class prop_##N { \
public: \
  operator T() __attribute__((noinline)); \
  prop_##N &operator=(T) __attribute__((noinline)); \
}

#define AS3_METH_DEF(R, N, A) \
  R N A __attribute__((noinline))

#define AS3_CONSTR_DEF(N, A) static N _new A

#define AS3_PROP(N) internal::prop_##N N
#define AS3_IPROP(N) internal::iprop_##N N

#define AS3_PROP_IMPL(N, NS) /* NS::internal::prop_##N NS::N */

#define AS3_BASE_IMPL(NS, N, NS3, N3) \
AS3_NS::Class NS N::internal::getClosure() { \
  AS3_MAYBE_DELEGATE((AS3_NS::Class (*)())(&NS N::internal::getClosure), 4); \
  unsigned eid; \
  unsigned id; \
  inline_as3 ("try { import " NS3 N3 "; %1 = as3_valAcquireId(" NS3 N3 "); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid), "=r"(id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  AS3_NS::Class res; \
  ((AS3_NS::internal::id_holder *)&res)->_id = id; \
  return res; \
} \
NS N::N(const NS N &val) { \
  _id = ((AS3_NS::internal::id_holder *)&val)->_id; \
  AS3_NS::var::internal::acquire(_id); \
} \
NS N::N(var val) { \
  operator=(val); \
} \
NS N &NS N::operator=(const NS N &val) { \
  unsigned id = ((AS3_NS::internal::id_holder *)&val)->_id; \
  AS3_NS::var::internal::acquire(id); \
  AS3_NS::var::internal::release(_id); \
  _id = id; \
  return *this; \
} \
NS N &NS N::operator=(var val) { \
  AS3_MAYBE_DELEGATE((NS N &(NS N::*)(var))(&NS N::operator=), 8); \
  unsigned eid; \
  unsigned id; \
  inline_as3 ("try { import " NS3 N3 "; var val:" N3 " = as3_id2rcv[%2].val; %1 = as3_valAcquireId(val); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid), "=r"(id) : "r"(((AS3_NS::internal::id_holder *)&val)->_id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  AS3_NS::var::internal::release(_id); \
  _id = id; \
  return *this; \
}

#define AS3_PROP_SET_SIMPL_(T, N, NS, N3, NS3) \
NS::internal::prop_##N &NS::internal::prop_##N::operator=(T val) { \
  AS3_MAYBE_DELEGATE((NS::internal::prop_##N &(NS::internal::prop_##N::*)(T))(&NS::internal::prop_##N::operator=), 4 + AS3_ARG_SIZE(T)); \
  unsigned eid; \
  inline_as3 ("try { import " NS3 "; " N3 " = %1; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid) : "r"(val)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  return *this; \
}
#define AS3_PROP_SET_SIMPL(T, N, NS, N3, NS3) \
AS3_PROP_SET_SIMPL_(T, N, NS, N3, NS3); \
NS::internal::prop_##N NS::N
#define AS3_PROP_GET_SIMPL_(T, N, NS, N3, NS3) \
NS::internal::prop_##N::operator T() { \
  AS3_MAYBE_DELEGATE((T (NS::internal::prop_##N::*)())(&NS::internal::prop_##N::operator T), 4); \
  unsigned eid; \
  T res; \
  inline_as3 ("try { import " NS3 "; %1 = " N3 "; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid), "=r"(res)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  return res; \
}
#define AS3_PROP_GET_SIMPL(T, N, NS, N3, NS3) \
AS3_PROP_GET_SIMPL_(T, N, NS, N3, NS3); \
NS::internal::prop_##N NS::N
#define AS3_PROP_SET_GET_SIMPL_(T, N, NS, N3, NS3) \
AS3_PROP_SET_SIMPL_(T, N, NS, N3, NS3); \
AS3_PROP_GET_SIMPL_(T, N, NS, N3, NS3)
#define AS3_PROP_SET_GET_SIMPL(T, N, NS, N3, NS3) \
AS3_PROP_SET_GET_SIMPL_(T, N, NS, N3, NS3); \
NS::internal::prop_##N NS::N

#define AS3_PROP_SET_IMPL_(T, N, NS, N3, NS3) \
NS::internal::prop_##N &NS::internal::prop_##N::operator=(T val) { \
  AS3_MAYBE_DELEGATE((NS::internal::prop_##N &(NS::internal::prop_##N::*)(T))(&NS::internal::prop_##N::operator=), 8); \
  unsigned eid; \
  unsigned id = ((AS3_NS::internal::id_holder *)&val)->_id; \
  inline_as3 ("try { import " NS3 "; " N3 " = as3_id2rcv[%1].val; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid) : "r"(id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  return *this; \
}
#define AS3_PROP_GET_IMPL_(T, N, NS, N3, NS3) \
NS::internal::prop_##N::operator T() { \
  AS3_MAYBE_DELEGATE((T (NS::internal::prop_##N::*)())(&NS::internal::prop_##N::operator T), 8); \
  unsigned eid; \
  unsigned id; \
  inline_as3 ("try { import " NS3 "; %1 = as3_valAcquireId(" N3 "); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid), "=r"(id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  T res; \
  ((AS3_NS::internal::id_holder *)&res)->_id = id; \
  return res; \
}
#define AS3_PROP_SET_IMPL(T, N, NS, N3, NS3) \
AS3_PROP_SET_IMPL_(T, N, NS, N3, NS3); \
NS::internal::prop_##N NS::N
#define AS3_PROP_GET_IMPL(T, N, NS, N3, NS3) \
AS3_PROP_GET_IMPL_(T, N, NS, N3, NS3); \
NS::internal::prop_##N NS::N
#define AS3_PROP_SET_GET_IMPL_(T, N, NS, N3, NS3) \
AS3_PROP_SET_IMPL_(T, N, NS, N3, NS3); \
AS3_PROP_GET_IMPL_(T, N, NS, N3, NS3)
#define AS3_PROP_SET_GET_IMPL(T, N, NS, N3, NS3) \
AS3_PROP_SET_GET_IMPL_(T, N, NS, N3, NS3); \
NS::internal::prop_##N NS::N

#define AS3_IMETH_IMPL(T, NS, C, N, A, I, AS) T NS C::internal::instance::N A { \
  AS3_MAYBE_DELEGATE((T (NS C::internal::instance::*)A)(&NS C::internal::instance::N), AS); \
I \
}
#define AS3_METH_IMPL(T, N, A, I, AS) T N A { \
  AS3_MAYBE_DELEGATE((T (*)A)(&N), AS); \
I \
}
#define AS3_CONSTR_IMPL(NS, N, A, I, AS) NS N NS N::_new A { \
  AS3_MAYBE_DELEGATE((NS N (*)A)(&NS N::_new), AS); \
I \
}

#define AS3_IPROP_TO_ID(NS, C, N) \
  ((AS3_NS::internal::id_holder *)((char *)this - __builtin_offsetof(NS C::internal::instance, N)))->_id

#define AS3_IPROP_SET_SIMPL(T, N, NS, C, N3) \
NS C::internal::iprop_##N &NS C::internal::iprop_##N::operator=(T val) { \
  AS3_MAYBE_DELEGATE((NS C::internal::iprop_##N &(NS C::internal::iprop_##N::*)(T))(&NS C::internal::iprop_##N::operator=), 4 + AS3_ARG_SIZE(T)); \
  unsigned eid; \
  unsigned _id = ((AS3_NS::internal::id_holder *)((char *)this - __builtin_offsetof(NS C::internal::instance, N)))->_id; \
  inline_as3 ("try { as3_id2rcv[%1].val." N3 " = %2; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid) : "r"(_id), "r"(val)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  return *this; \
}
#define AS3_IPROP_GET_SIMPL(T, N, NS, C, N3) \
NS C::internal::iprop_##N::operator T() { \
  AS3_MAYBE_DELEGATE((T (NS C::internal::iprop_##N::*)())(&NS C::internal::iprop_##N::operator T), 4); \
  unsigned eid; \
  unsigned _id = ((AS3_NS::internal::id_holder *)((char *)this - __builtin_offsetof(NS C::internal::instance, N)))->_id; \
  T res; \
  inline_as3 ("try { %1 = as3_id2rcv[%2].val." N3 "; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid), "=r"(res) : "r"(_id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  return res; \
}
#define AS3_IPROP_SET_GET_SIMPL(T, N, NS, C, N3) \
AS3_IPROP_SET_SIMPL(T, N, NS, C, N3); \
AS3_IPROP_GET_SIMPL(T, N, NS, C, N3)

#define AS3_IPROP_SET_IMPL(T, N, NS, C, N3) \
NS C::internal::iprop_##N &NS C::internal::iprop_##N::operator=(T val) { \
  AS3_MAYBE_DELEGATE((NS C::internal::iprop_##N &(NS C::internal::iprop_##N::*)(T))(&NS C::internal::iprop_##N::operator=), 8); \
  unsigned eid; \
  unsigned _id = ((AS3_NS::internal::id_holder *)((char *)this - __builtin_offsetof(NS C::internal::instance, N)))->_id; \
  unsigned id = ((AS3_NS::internal::id_holder *)&val)->_id; \
  inline_as3 ("try { as3_id2rcv[%1].val." N3 " = as3_id2rcv[%2].val; %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid) : "r"(_id), "r"(id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  return *this; \
}
#define AS3_IPROP_GET_IMPL(T, N, NS, C, N3) \
NS C::internal::iprop_##N::operator T() { \
  AS3_MAYBE_DELEGATE((T (NS C::internal::iprop_##N::*)())(&NS C::internal::iprop_##N::operator T), 8); \
  unsigned eid; \
  unsigned _id = ((AS3_NS::internal::id_holder *)((char *)this - __builtin_offsetof(NS C::internal::instance, N)))->_id; \
  unsigned id; \
  inline_as3 ("try { %1 = as3_valAcquireId(as3_id2rcv[%2].val." N3 "); %0 = -1 } catch(e:*) { %0 = as3_valAcquireId(e) }" : \
      "=r"(eid), "=r"(id) : "r"(_id)); \
  if(eid != -1U) { var e; AS3_GET_ID(e) = eid; throw e; } \
  T res; \
  ((AS3_NS::internal::id_holder *)&res)->_id = id; \
  return res; \
}
#define AS3_IPROP_SET_GET_IMPL(T, N, NS, C, N3) \
AS3_IPROP_SET_IMPL(T, N, NS, C, N3); \
AS3_IPROP_GET_IMPL(T, N, NS, C, N3)

#endif

#define AS3_UI 1
#define AS3_NS AS3::ui

namespace AS3 {
namespace ui {
#ifdef AS3_IMPL
#define _AS3_IMPL
#undef AS3_IMPL
#endif
#ifdef AS3WIG_USE_BUILTIN_DEFS
  #include <AS3++/builtin.h>
  #include <AS3++/playerglobal.h>
#else
  #include "AGAL_internal.h"
#endif
#ifdef _AS3_IMPL
#define AS3_IMPL
#undef _AS3_IMPL
#ifdef AS3WIG_USE_BUILTIN_DEFS
  #include <AS3++/builtin.h>
  #include <AS3++/playerglobal.h>
#else
  #include "AGAL_internal.h"
#endif
#endif
}
}

#undef AS3_NS
#undef AS3_UI

#define AS3_UI 0
#define AS3_NS AS3::local

namespace AS3 {
namespace local {
#ifdef AS3_IMPL
#define _AS3_IMPL 
#undef AS3_IMPL
#endif
#ifdef AS3WIG_USE_BUILTIN_DEFS
  #include <AS3++/builtin.h>
  #include <AS3++/playerglobal.h>
#else
  #include "AGAL_internal.h"
#endif
#ifdef _AS3_IMPL
#define AS3_IMPL
#undef _AS3_IMPL
#ifdef AS3WIG_USE_BUILTIN_DEFS
  #include <AS3++/builtin.h>
  #include <AS3++/playerglobal.h>
#else
  #include "AGAL_internal.h"
#endif
#endif
}
}

#undef AS3_NS
#undef AS3_UI
