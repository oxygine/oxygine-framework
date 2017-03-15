
/*! \mainpage Winnie Alloc Library ver. 0.3

  woowoowoow 'at' bk.ru
  
  winnie_alloc do not support reenterancy to Winnie::*** functions. So, if you need memory allocation in 
  multithreading application, you should protect new/delete operations by critical sections by yourself.
  See new_delete.cpp file from new_delete project for example.
 */

//(c) Winnie  woowoowoow 'at' bk.ru
//fast memory allocation routines.

#include "winnie_alloc.h"
#include "system_alloc.h"
#include <new> //for std::bad_alloc
#include <string.h>
#include <stdio.h>

#if WINNIE_ALLOC_USE_MALLOC_THRESHOLD
# include <malloc.h> //for _msize
#endif

namespace Winnie
{

namespace Detail
{

using namespace Winnie::Config;

const size_t first_os_block_size = 1 << first_os_block_size_log; 
//const size_t max_os_block_num = 1 + 0x800000000/first_os_block_size; 
const size_t max_os_block_num = 10240; 

struct Header
{
	union 
	{
		Header *next; //in free block, next free block
		struct //in allocated blocks. 
		{
			unsigned char is_malloced; //was allocated by stdlib::malloc
			unsigned char list_num; //if not, number of list in list of lists
		};
	};
};

//Big blocks consumed from OS.
size_t os_block_num;
void *os_blocks[max_os_block_num];

//last element is always NULL (for case when user call Winnie::Alloc
//(0x80000001) and list_num will be equal 32.
Header *free_blocks_lists[max_lists_num+1];

//number of allocated blocks in each list. (for statistic)
//in Winnie::Alloc num_allocated_blocks[i] is increased,
//in Winnie::Free num_allocated_blocks[i] is decreased,
size_t num_allocated_blocks[max_lists_num];

//number of allocations in each list(for statistic). 
//num_block_allocations[i] >= num_allocated_blocks[i]
//in Winnie::Alloc num_allocated_blocks[i] is increased.
size_t num_block_allocations[max_lists_num];

const unsigned int BitTable[256] = 
{
0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,
5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

inline unsigned GetNextPowerOfTwo(size_t k)
{
  --k;

  if (k < 256)
    return BitTable[k];

  if (k < 256*256)
    return BitTable[k>>8]+8;

  if (k < 256*256*256)
    return BitTable[k>>16]+16;

  return BitTable[k>>24]+24;
}


void AddBlockToList(void *p_memory, unsigned list_num)
{
  Header *p_header = (Header *)p_memory; //non initialized header
  p_header->next = free_blocks_lists[list_num];
  free_blocks_lists[list_num] = p_header;
}

Header *DetachFirstBlock(unsigned list_num)
{
  Header *p_header= free_blocks_lists[list_num];
  free_blocks_lists[list_num]= free_blocks_lists[list_num]->next;
  p_header->list_num = list_num;
  p_header->is_malloced= 0;
  return p_header; 
}

void SplitBlockAndAddToLists(void *p_raw_memory, unsigned first_list_num, unsigned memory_log_size)
{
  void *p_memory = (char*)p_raw_memory + (1U << (memory_log_size-1)); //set pointer to second half
  for (unsigned i = memory_log_size-1; i >= first_list_num; --i)
  {
    AddBlockToList(p_memory, i);
    p_memory = (char*)p_memory - (1U<<(i-1)); //leave other half for next iteration
  }

  AddBlockToList(p_raw_memory, first_list_num);
}

void *OnOutOfMemory()
{
#if WINNIE_ALLOC_THROW_STD_BADALLOC
  throw std::bad_alloc(); 
#else
  return NULL;
#endif
};


bool FindNewBlockForList(unsigned list_num)
{
  if (list_num >= max_lists_num)
    return false;

  unsigned i = list_num + 1;
  for (; i<max_lists_num; ++i)
  {
    if (free_blocks_lists[i])
      break;
  }

  void *result;

  if (i != max_lists_num)
  { //we have found bigger block in other list.
    result = DetachFirstBlock(i);
    SplitBlockAndAddToLists(result, list_num, i);
  }
  else
  { //we need to get new block from OS.
    unsigned size_of_new_os_block_log = 
      (first_os_block_size_log > list_num)?
      first_os_block_size_log  :
      list_num;

	  if (os_block_num == max_os_block_num)
		  return false;

    result = SystemAlloc( 1U << size_of_new_os_block_log );
    if (!result)
      return false;
    SplitBlockAndAddToLists(result, list_num, size_of_new_os_block_log);
    os_blocks[os_block_num++] = result;    
	//printf("os_block_num: %d\n", os_block_num);
  }

  return true;
}

}// namespace Detail


void GetAllocInfo(Statistic &statistic)
{
  using namespace Detail;
  for (unsigned i=0; i<max_lists_num; ++i)
  {
    statistic.num_allocated_blocks[i] = num_allocated_blocks[i];
    statistic.num_block_allocations[i] = num_block_allocations[i];

    int num_free_blocks = 0;
    Header *p = free_blocks_lists[i];
    while (p) 
    {
      ++num_free_blocks;
      p = p->next;
    }

    statistic.num_free_blocks[i] = num_free_blocks;    
  }
}


void *Alloc(size_t size)
{
  using namespace Detail;
  const unsigned list_num = GetNextPowerOfTwo(
	  size+sizeof(Header)); 

#if WINNIE_ALLOC_USE_MALLOC_THRESHOLD
  if (size >= malloc_threshold)
  {
	  Header *p_header = (Header *) malloc(size+sizeof(Header));
    if (!p_header)
    {
      return OnOutOfMemory();
    }
	  p_header->is_malloced = 1;
	  return ++p_header;
  }
  else
#endif
  {
	  if (!free_blocks_lists[list_num])
	  {
		  if (!FindNewBlockForList(list_num))
		  {
        return OnOutOfMemory();
		  }
	  }

#if WINNIE_ALLOC_STATISTIC_ENABLED
	  num_allocated_blocks[list_num]++;
    num_block_allocations[list_num]++;
#endif

	  return 1 + DetachFirstBlock(list_num);
  }
}


void Free(void *p) throw ()
{
  if (!p)
    return; 

  using namespace Detail;

  Header *p_header = (Header *)p - 1;
#if WINNIE_ALLOC_USE_MALLOC_THRESHOLD
  if (p_header->is_malloced)
  {
	  free(p_header);
  }
  else
#endif
  {
	  unsigned list_num = p_header->list_num;
	  AddBlockToList(p_header, list_num);
#if WINNIE_ALLOC_STATISTIC_ENABLED
	  num_allocated_blocks[list_num]--;
#endif
  }
}

void *Realloc(void *p, size_t size)
{
  using namespace Detail;
  Header *p_header = (Header *)p - 1;
#if WINNIE_ALLOC_USE_MALLOC_THRESHOLD
  if (p_header->is_malloced)
  { 
    //this branch is not heavily tested.
    if (size < malloc_threshold)
    {
      void *p_new_memory = Alloc(size);
      memcpy(p_new_memory, p, size);
      free(p_header);
      return p_new_memory;
    }
    else
    {
      Header *p_new_header = (Header *)realloc(p_header, size+sizeof(Header));
      if (!p_new_header)
      {
        return OnOutOfMemory();
      }
	    p_new_header->is_malloced = 1;
	    return ++p_new_header;
    }   
    ////if (size < malloc_threshold)
    ////{
    ////  //this branch use Microsoft specific `_msize`.
    ////  size_t old_size = _msize(p);
    ////  assert(old_size >= malloc_threshold);
    ////  void *new_p= Winnie::Alloc(size);
    ////  Header *new_p_header = (Header *)new_p;
    ////  //`old_size` if always bigger than `size`, 
    ////  //because (size < malloc_threshold) && (malloc_threshold <= old_size)
    ////  memcpy(new_p, p, size);
    ////  free(p_header);
    ////  return new_p;
    ////}
    ////else

    //{
    //  void *new_p = realloc(p_header, size+sizeof(Header));
    //  if (!new_p)
    //  {
    //    return OnOutOfMemory<Config::throw_std_bad_alloc>::F();
    //  }
    //  Header *new_p_header = (Header *)new_p;
    //  new_p_header->is_malloced = 1;
    //  return ++new_p_header;
    //}
  }
  else
#endif
  {
    const unsigned new_list_num = GetNextPowerOfTwo(
  	  size+sizeof(Header)); 
    if (p_header->list_num != new_list_num)
    {
      size_t old_size = 1U << p_header->list_num;
      void *new_p= Winnie::Alloc(size);
      if (size <= old_size)
        memcpy(new_p, p, size);
      else
        memcpy(new_p, p, old_size);
      Winnie::Free(p);
      return new_p;
    }
    else
      return p;
  }
}

size_t MSize(void *p)
{
  using namespace Detail;
  Header *p_header = (Header *)p - 1;
#if WINNIE_ALLOC_USE_MALLOC_THRESHOLD
  if (p_header->is_malloced)
  {
    return _msize(p);
  }
  else
#endif
  {
    return 1U<<p_header->list_num;
  }
}

void DesroyHeap()
{
  using namespace Detail;

  for (unsigned i=0; i<max_os_block_num; ++i)
    if (os_blocks[i])
      SystemFree(os_blocks[i]);

  memset(os_blocks, 0, sizeof(os_blocks));
  os_block_num = 0;
  memset(free_blocks_lists, 0, sizeof(free_blocks_lists));
  memset(num_allocated_blocks, 0, sizeof(num_allocated_blocks));
  memset(num_block_allocations, 0, sizeof(num_block_allocations));
}

} //namespace Winnie

