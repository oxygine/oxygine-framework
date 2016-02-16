
/*

Closure library 1.1
file "closure.h"

Written by Ivan Yankov aka _Winnie (woowoowoow@bk.ru)
Many thanks to Wolfhound

*/



#ifndef CLOSURE_HEADER_
#define CLOSURE_HEADER_

#ifdef _MSC_VER
#  pragma once
#endif

#if !defined(__S3E__) || !defined(OXYGINE_EDITOR)
#define CLOSURE_FUNCTION 1
#endif

#ifdef CLOSURE_FUNCTION
#	include <functional>
#	include <memory>
#endif

#define TEMPLATE_PARAM_LIST class R
#define PARAM_TYPE_LIST 
#define PARAM_TYPE_LIST_COMMA 
#define PARAM_FORM_ARG_LIST 
#define PARAM_FORM_ARG_LIST_COMMA 
#define PARAM_ARG_LIST 
#define PARAM_ARG_LIST_COMMA 
#define CLOSURE_NUM Closure0
#include "closure_impl.h"

#define TEMPLATE_PARAM_LIST class R, class P0
#define PARAM_TYPE_LIST P0
#define PARAM_TYPE_LIST_COMMA , P0
#define PARAM_FORM_ARG_LIST P0 p0
#define PARAM_FORM_ARG_LIST_COMMA , P0 p0
#define PARAM_ARG_LIST p0
#define PARAM_ARG_LIST_COMMA , p0
#define CLOSURE_NUM Closure1
#include "closure_impl.h"

#define TEMPLATE_PARAM_LIST class R, class P0, class P1
#define PARAM_TYPE_LIST P0, P1
#define PARAM_TYPE_LIST_COMMA , P0, P1
#define PARAM_FORM_ARG_LIST P0 p0, P1 p1 
#define PARAM_FORM_ARG_LIST_COMMA , P0 p0, P1 p1
#define PARAM_ARG_LIST p0, p1
#define PARAM_ARG_LIST_COMMA , p0, p1
#define CLOSURE_NUM Closure2
#include "closure_impl.h"

#define TEMPLATE_PARAM_LIST class R, class P0, class P1, class P2
#define PARAM_TYPE_LIST P0, P1, P2
#define PARAM_TYPE_LIST_COMMA , P0, P1, P2
#define PARAM_FORM_ARG_LIST P0 p0, P1 p1, P2 p2
#define PARAM_FORM_ARG_LIST_COMMA , P0 p0, P1 p1, P2 p2
#define PARAM_ARG_LIST p0, p1, p2
#define PARAM_ARG_LIST_COMMA , p0, p1, p2
#define CLOSURE_NUM Closure3
#include "closure_impl.h"

#define TEMPLATE_PARAM_LIST class R, class P0, class P1, class P2, class P3
#define PARAM_TYPE_LIST P0, P1, P2, P3
#define PARAM_TYPE_LIST_COMMA , P0, P1, P2, P3
#define PARAM_FORM_ARG_LIST P0 p0, P1 p1, P2 p2, P3 p3
#define PARAM_FORM_ARG_LIST_COMMA , P0 p0, P1 p1, P2 p2, P3 p3
#define PARAM_ARG_LIST p0, p1, p2, p3
#define PARAM_ARG_LIST_COMMA , p0, p1, p2, p3
#define CLOSURE_NUM Closure4
#include "closure_impl.h"

#define CLOSURE(PTR, MEM_PTR) (detail::CreateClosure(MEM_PTR).Init<MEM_PTR>(PTR))

#if CLOSURE_FUNCTION
#	define CLOSUREF(F) (detail::CreateClosureF(F))
#endif

#endif

