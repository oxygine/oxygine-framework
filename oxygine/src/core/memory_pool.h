#pragma once
#include "oxygine_include.h"
/*
#include <stddef.h>
#include <malloc.h>
namespace oxygine
{
	namespace memory
	{
		
		class memory_pool
		{
		public:
			memory_pool(int blockSize, int count);

			void *alloc();
			void free(void *ptr);

			size_t getBlockSize()const {return _blockSize;}

		protected:
			void new_block();

			size_t _blockSize;
			int _count;
			void *_first_free;
		};
		
		template<class T, int count>
		class classAllocator
		{
		public:
			typedef T type;
			enum {COUNT = count};

			void *operator new(size_t size)
			{
				if (_pool.getBlockSize() < size) 
					return malloc(size);
				return _pool.alloc();
			}
			void operator delete(void *data, size_t size)
			{
				if (_pool.getBlockSize() < size) 
					return free(data);
				_pool.free(data);
			}

		protected:
			static memory_pool _pool;
		};
	}	

#define DECLARE_ALLOCATOR(CLASS, COUNT) typedef memory::classAllocator<CLASS, COUNT> Allocator##CLASS
#define INSTANTIATE_ALLOCATOR(CLASS) memory::memory_pool Allocator##CLASS::_pool(sizeof(CLASS), Allocator##CLASS::COUNT)
 
}
*/