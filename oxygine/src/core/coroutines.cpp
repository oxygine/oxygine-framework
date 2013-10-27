#include "coroutines.h"
#include "Tweener.h"
#include "Clock.h"

#if __S3E__
#include "IwFibre.h"
#elif OXYGINE_SDL
#include "greenlet.h"
#define GREENLETS 1
#endif
//#include "greenlet.h"

namespace oxygine
{
	namespace coroutine
	{
		int fiberPassData = 0;

		handle current()
		{
#if __S3E__
			handle f = (handle)IwFibreGetCurrent();
			return f;
#else
			OX_ASSERT(!"not implemented");
			return 0;			
#endif			
		}

		handle create(fiberFunc func, int stackSize, void *param)
		{
#if __S3E__
			handle f = (handle)IwFibreCreate(func, stackSize, param, 0);	
			return f;
#elif GREENLETS
			handle f = (handle)greenlet_create((gr_callback)func);	
			return f;
#else

#endif
			return 0;
		}

		void terminate(handle fiber, int data)
		{
#if __S3E__
			if (data)
				resume(fiber, data);
			IwFibreJoin((IwFibre*)fiber);
			IwFibreDelete((IwFibre*)fiber);
#else
			//assert(0);
#endif
		}

		int resume(handle fiber, int data)
		{
			fiberPassData = data;
			int r = 0;
#if __S3E__
			IwFibreYield((IwFibre*)fiber);
#elif GREENLETS
			if (!fiber)
				fiber = greenlet_getparent();
			r = greenlet_switch((greenlet*)fiber, data);
#else

#endif

			if (fiber == 0)
			{
				if (fiberPassData == 1)
				{
					throw FiberExit();
				}
			}

			return r;
		}
	}
}