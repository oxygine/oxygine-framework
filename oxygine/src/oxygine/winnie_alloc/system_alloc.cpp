//system_alloc.cpp
//OS-specific memory allocation functions
//this is part of winnie_alloc library
//you may rewrite this functions if you like

#include "system_alloc.h"

void *SystemAlloc(size_t size)
{
  return malloc(size);
}

void SystemFree(void *p)
{
  free(p);
}

