
#ifndef SYSTEM_ALLOC_HEADER_
#define SYSTEM_ALLOC_HEADER_

//system_alloc.h
//OS-specific memory allocation functions

#ifdef _MSC_VER
#  pragma once
#endif

#include "stdlib.h"

void *SystemAlloc(size_t size);
void SystemFree(void *p);

#endif
