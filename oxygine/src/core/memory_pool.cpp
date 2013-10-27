#include "memory_pool.h"
//#include <malloc.h>
namespace oxygine
{
    /*
	namespace memory
	{
		memory_pool::memory_pool(int blockSize, int count):_first_free(0), _blockSize(blockSize), _count(count)
		{
		}

		void memory_pool::new_block() 
		{
			const int LARGE_BLOCK_SIZE = _blockSize * _count;
			char *block = (char*)malloc(LARGE_BLOCK_SIZE);
			for (int i = 0; i < _count; ++i) 
			{
				*(void**)(block + i * _blockSize) = (i != _count - 1) ? (block + (i+1) * _blockSize) : 0;
			}
			_first_free = block;
		}

		void *memory_pool::alloc() 
		{
			if (!_first_free)
				new_block();

			void *result = _first_free;
			_first_free = *(void **)_first_free;
			return result;
		}

		void memory_pool::free(void *ptr) 
		{
			*(void**)ptr = _first_free;
			_first_free = ptr;
		}
	}
     */
}