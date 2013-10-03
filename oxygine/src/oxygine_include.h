#pragma once
#ifndef OXYGINE_INCLUDE
#define OXYGINE_INCLUDE

#if __S3E__
	#define OXYGINE_MARMALADE 1
	#ifdef IW_DEBUG
		#define OX_DEBUG 1
	#endif
#else
	#define OXYGINE_SDL 1
#endif


#if _DEBUG || DEBUG
	#ifndef OX_DEBUG
		#define OX_DEBUG 1
	#endif
#endif



#ifndef OX_DEBUG
	#define USE_MEMORY_POOL
	#define OBJECT_POOL_ALLOCATOR
#else	
#endif



#define OX_ASSERT(x) if (!(x)) oxygine::log::error("Assert! %d %s", __LINE__, __FILE__); (assert(x))
#define OX_LOG(format, args) {}


#endif
