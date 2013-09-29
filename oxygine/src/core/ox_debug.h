#pragma once
#include "oxygine_include.h"
#include <assert.h>
#include "log.h"

#ifdef __S3E__
#ifdef IW_DEBUG
#define OX_DEBUG 1
#endif

#else
#ifdef _DEBUG
#define OX_DEBUG 1
#endif

#endif

#define OX_ASSERT(x) if (!(x)) oxygine::log::error("Assert! %d %s", __LINE__, __FILE__); (assert(x))
/*
#ifdef OX_DEBUG

#else
#define OX_ASSERT(x) ((void)0)
#endif
*/
#define OX_LOG(format, args) {}
