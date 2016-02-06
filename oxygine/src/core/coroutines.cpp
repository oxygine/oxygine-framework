#include "coroutines.h"

#if __S3E__
#define OXYGINE_COROUTINES
#include "IwFibre.h"
#elif OXYGINE_EDITOR
#elif OXYGINE_SDL
#ifndef __APPLE__
//#include "greenlet.h"
#else
#endif
#else
#endif

#define GREENLETS 0

namespace oxygine
{
    namespace coroutine
    {
        int fiberPassData = 0;

        void init()
        {
#if __S3E__
#elif GREENLETS
            greenlets_init();
#else
#endif
        }

        handle current()
        {
#if __S3E__
            handle f = (handle)IwFibreGetCurrent();
            return f;
#elif GREENLETS
            handle f = (handle)greenlet_getcurrent();
            return f;
#else
            OX_ASSERT(!"not implemented");
            return 0;
#endif
        }

        handle create(fiberFunc func, int stackSize, void* param)
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
#elif GREENLETS
            greenlet_kill((greenlet*)fiber);
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

        bool isdead(handle fiber)
        {
#if __S3E__
            return !IwFibreIsRunning((IwFibre*) fiber);
#elif GREENLETS
            return greenlet_isdead((greenlet*) fiber) != 0;
#else
            return false;
#endif
        }
    }
}
