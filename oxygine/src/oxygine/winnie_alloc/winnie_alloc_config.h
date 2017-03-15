
#ifndef WINNIE_ALLOC_CONFIG_HEADER_
#define WINNIE_ALLOC_CONFIG_HEADER_

#ifdef _MSC_VER
#  pragma once
#endif

#include <stddef.h>

namespace Winnie
{

  /**
  @see WINNIE_ALLOC_USE_MALLOC_THRESHOLD
  @see WINNIE_ALLOC_STATISTIC_ENABLED
  @see WINNIE_ALLOC_THROW_STD_BADALLOC 
  */
namespace Config
{
  /*
  winnie_alloc do not support reenterancy to Winnie::*** functions. So, if your need memory allocation in 
  multithreading application, you shold your protect new/delete operations by critical sections by yorself.
  See new_delete.cpp file from walloc_test project for example.
  */

  /**winnie_alloc library takes big chunks of memory from system.
  this is (logarithm of) minimal size of such block: */
  const unsigned first_os_block_size_log = 14; //1 MB.

  /**when winnie_alloc can't allocate memory, it should return NULL or throw std::bad_alloc?*/
  #define WINNIE_ALLOC_THROW_STD_BADALLOC 1

  /**
  use Winnie::Config::malloc_threshold or not?
  If USE_MALLOC_THRESHOLD  is 0, then all refereces to malloc/free are removed and parameter malloc_threshold not used
  */
  #define WINNIE_ALLOC_USE_MALLOC_THRESHOLD 0

  /** if requested size of memory is greater or equal `malloc_threshold`, then standart function malloc used. 
  winnie_alloc rounds size of every requested block to the next power of two, so when user allocate very 
  big blocks,  winnie_alloc can exhaust 32-bits memory address space. You can use malloc for big blocks. 
  Winnie recommends make frequently allocated blocks duty of winnie_alloc library, 
  and big unfrequently allocated blocks leave for std::malloc */

  const size_t malloc_threshold = 1024;
  
  /** enable counting of allocated blocks or not? \see GetAllocInfo */
  #define WINNIE_ALLOC_STATISTIC_ENABLED 0
}  
}

#endif
