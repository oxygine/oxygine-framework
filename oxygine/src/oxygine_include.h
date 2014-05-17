#pragma once
#ifndef OXYGINE_INCLUDE
#define OXYGINE_INCLUDE
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#if __S3E__
	#define OXYGINE_MARMALADE 1
	#ifdef IW_DEBUG
		#define OX_DEBUG 1
	#endif
#else
	#define OXYGINE_SDL 1
	#ifdef WIN32
		#ifndef _CRT_SECURE_NO_WARNINGS
			#define _CRT_SECURE_NO_WARNINGS
		#endif
	#endif
#endif


#if _DEBUG || DEBUG
	#ifndef OX_DEBUG
		#define OX_DEBUG 1
	#endif
#endif



#ifndef OX_DEBUG
	#define USE_MEMORY_POOL 1
	#define OBJECT_POOL_ALLOCATOR 1
#endif

#if OX_DEBUG
	#define OXYGINE_DEBUG_TRACE_LEAKS 1
	#define OXYGINE_DEBUG_T2P 1
	#define OXYGINE_DEBUG_SAFECAST 1
	#define OXYGINE_TRACE_VIDEO_STATS 1
#endif

#define OXYGINE_ASSERT2LOG 1

#if OXYGINE_ASSERT2LOG
namespace oxygine{namespace log{void error(const char *format, ...);}}
#ifdef OXYGINE_QT
#define OX_ASSERT(x) if (!(x)) {__asm("int3");oxygine::log::error("Assert! %d %s", __LINE__, __FILE__); }(assert(x))
#else
#define OX_ASSERT(x) if (!(x)) {oxygine::log::error("Assert! %d %s", __LINE__, __FILE__);}(assert(x))
#endif
#else
#define OX_ASSERT(x) (if (!(x)) oxygine::log::error("Assert! %d %s", __LINE__, __FILE__))
#endif

#define OXYGINE_HAS_RESTORE

#endif
