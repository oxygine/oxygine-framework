#pragma once
#ifndef OXYGINE_INCLUDE
#define OXYGINE_INCLUDE
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


//#define OXYGINE_NO_YEILD 1

#if __S3E__
	#define OXYGINE_MARMALADE 1
	#ifdef IW_DEBUG
		#define OX_DEBUG 1
	#endif
#elif EMSCRIPTEN
	#define OXYGINE_EMSCRIPTEN 1
	#ifndef NDEBUG
		#define OX_DEBUG 1
	#endif // DEBUG	
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


#define DYNAMIC_OBJECT_NAME 1


#ifndef OX_DEBUG
	#ifndef EMSCRIPTEN
		#define USE_MEMORY_POOL 1
		#define OBJECT_POOL_ALLOCATOR 1
	#endif
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
#elif EMSCRIPTEN
#define OX_ASSERT(x) if (!(x)) {oxygine::log::error("Assert! %d %s", __LINE__, __FILE__);}
#else
#define OX_ASSERT(x) if (!(x)) {oxygine::log::error("Assert! %d %s", __LINE__, __FILE__);}(assert(x))
#endif
#else
#define OX_ASSERT(x) (if (!(x)) oxygine::log::error("Assert! %d %s", __LINE__, __FILE__))
#endif

#define OXYGINE_HAS_RESTORE

#define OXYGINE_RENDERER 2

#ifdef __GNUC__
#define OXYGINE_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define OXYGINE_DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define OXYGINE_DEPRECATED
#endif


#ifdef _MSC_VER
#define OVERRIDE override
#else
#define OVERRIDE 
#endif


#endif //OXYGINE_INCLUDE
