

#ifndef WINNIE_ALLOC_HEADER_
#define WINNIE_ALLOC_HEADER_

//(c) Winnie  woowoowoow 'at' bk.ru
//fast memory allocation routines.

#ifdef _MSC_VER
#  pragma once
#endif

#include "winnie_alloc_config.h"
#include "stdlib.h"


namespace Winnie
{

  
/**
number of free block lists, which holds blocks of size 1, 2, 4, 8, ..., 2GB
*/
const unsigned max_lists_num = 31;

/**
allocation statistic info

i is in interval [0 .. max_lists_num - 1]
*/
struct Statistic
{
  /** num_allocated_blocks[i] shows how many (2^i)-sized blocks are allocated. */
  size_t num_allocated_blocks[max_lists_num];
  
  /** num_free_blocks[i] shows how many (2^i)-sized free blocks in pool. 
  in Winnie::Alloc num_allocated_blocks[i] is increased,
  in Winnie::Free num_allocated_blocks[i] is decreased*/
  size_t num_free_blocks[max_lists_num];

  /** num_block_allocatons[i] shows how many (2^i)-sized blocks were allocated. 
  in Winnie::Alloc num_allocated_blocks[i] is increased.  */
  size_t num_block_allocations[max_lists_num];
};


/**
return statistic info, if WINNIE_ALLOC_STATISTIC_ENABLED is true. \see WINNIE_ALLOC_STATISTIC_ENABLED
*/
void GetAllocInfo(Statistic &statistic);


/**
@return pointer to allocated block.  May return NULL or throw std::bad_alloc, if not enough memory 
\see Winnie::Config::throw_std_bad_alloc
@param size: if size` is bigger than malloc_threshold, standart malloc function used.
\see Winnie::Config::malloc_threshold

*/
void *Alloc(size_t size);


/**
frees a memory block, which was allocated whith Winnie::Alloc 
*/
void Free(void *p) throw ();

/**
Reallocate memory blocks
@param p Pointer to previously allocated memory block 
@param size New size in bytes 
The contents of the block are unchanged up to the shorter of the new and old sizes, 
although the new block can be in a different location
*/
void *Realloc(void *p, size_t size);

/**
Returns actual number of bytes in allocated block, 
which can be more than requested by user.
*/
size_t MSize(void *p);


/**
free all consumed from OS memory and return to zero-initialized state.
*/
void DesroyHeap();

}//namespace Winnie


#endif
