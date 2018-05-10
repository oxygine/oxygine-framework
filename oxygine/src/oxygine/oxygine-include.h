#pragma once
#ifndef OXYGINE_INCLUDE_HEADER
#define OXYGINE_INCLUDE_HEADER

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


#if __APPLE__
#include <TargetConditionals.h>
#endif

#if EMSCRIPTEN
#include <emscripten.h>
#endif


//Round everything to whole pixels during rendering.
// Helps to avoid artifacts in pixel art games
// when using textures with linearFilter="false"
// and fractional sprite coordinates or sprite scaling.
// Introduces some CPU overhead.
//#define OXYGINE_NO_SUBPIXEL_RENDERING 1


#ifdef __ANDROID__
#define HAVE_NPOT_RT()  (true)
#else
#define HAVE_NPOT_RT()  (true)
#endif

#define OX_HAS_GLOBAL_TF_SCALE 1

//#define OXYGINE_NO_YEILD 1

// Bypass SDL file functions and use plain fopen, fread etc.
//#define OXYGINE_FILESYSTEM_USE_STDIO 1


#ifdef EMSCRIPTEN
#   define OXYGINE_EMSCRIPTEN 1
#   define OXYGINE_SDL 1
#   ifndef NDEBUG
#       define OX_DEBUG 1
#   endif // DEBUG  
#elif OXYGINE_EDITOR
#   define OXYGINE_FILESYSTEM_USE_STDIO 1
#   include <qglobal.h>
#else
#   define OXYGINE_SDL 1
#   ifdef _WIN32
#       ifndef _CRT_SECURE_NO_WARNINGS
#           define _CRT_SECURE_NO_WARNINGS
#       endif
#   endif
#endif


#if _DEBUG || DEBUG
#   ifndef OX_DEBUG
#       define OX_DEBUG 1
#   endif
#endif


#define DYNAMIC_OBJECT_NAME 1


#ifndef OX_DEBUG
#   ifndef EMSCRIPTEN
#       define USE_MEMORY_POOL 1
#       define OBJECT_POOL_ALLOCATOR 1
#   endif
#endif

#if OX_DEBUG
#   define OXYGINE_DEBUG_TRACE_LEAKS 1
#   define OXYGINE_DEBUG_T2P 1
#   define OXYGINE_DEBUG_SAFECAST 1
#endif

#   define OXYGINE_TRACE_VIDEO_STATS 1

#define OXYGINE_ASSERT2LOG 1

#ifdef EMSCRIPTEN
void emscStackTrace();
#endif

namespace oxygine { namespace logs { void error(const char* format, ...); } }
namespace ox = oxygine;

#define OX_LOG_ERROR(x)     if (!(x)) {oxygine::logs::error("Assert! %s in %s:%d", #x, __FILE__, __LINE__);}


//assert without logs::error
#ifdef OXYGINE_QT
#   define OX_ASSERT_NL(x) { if (!(x)) __asm("int3"); Q_ASSERT(x);}
#elif !OX_DEBUG || EMSCRIPTEN
#   define OX_ASSERT_NL(x)
#else
#   define OX_ASSERT_NL(x) {assert(x);}
#endif


#if OXYGINE_ASSERT2LOG
#   define OX_ASSERT(x) {OX_LOG_ERROR(x); OX_ASSERT_NL(x);}
#else
#   define OX_ASSERT(x) {OX_ASSERT_NL(x);}
#endif

#define OXYGINE_HAS_RESTORE

#define OXYGINE_RENDERER 5

#define OXYGINE_VERSION 10

#ifdef __GNUC__
#   define OXYGINE_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#   define OXYGINE_DEPRECATED __declspec(deprecated)
#else
#   pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#   define OXYGINE_DEPRECATED
#endif


#ifdef _MSC_VER
#   define OVERRIDE override
#else
#   define OVERRIDE
#endif


#   if defined(_MSC_VER) || defined(__BORLANDC__)
typedef unsigned __int64 uint64;
typedef signed __int64 int64;
#   else
typedef unsigned long long uint64;
typedef signed long long int64;
#   endif

#ifdef OXYGINE_QT
#define INHERITED(CLASS) private: typedef Editor##CLASS inherited
#else
#define INHERITED(CLASS) private: typedef CLASS inherited
#endif

#ifndef EDITOR_INCLUDE
#define EDITOR_INCLUDE(CLASS)
#endif


#if ( (defined(_MSC_VER) && (_MSC_VER > 1800)) || (__cplusplus > 199711L))
#define OX_HAS_CPP11
#endif


#include "oxygine-forwards.h"
#endif