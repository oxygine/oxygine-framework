#pragma once
namespace oxygine
{
	namespace coroutine
	{
		class FiberExit
		{
		public:

		};

		typedef void* handle;

		typedef void* (*fiberFunc) (void* param);

		handle create(fiberFunc func, int stackSize, void *param);
		handle current();

		void terminate(handle fiber, int data = 0);
		int resume(handle fiber = 0, int data = 0);
	}
}