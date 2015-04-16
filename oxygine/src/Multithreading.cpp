#include "Multithreading.h"
#include "core/Mem2Native.h"
#include "res/Resources.h"
#include "res/Resource.h"
#include "Stage.h"
#include "res/CreateResourceContext.h"

#ifdef __S3E__
#include "s3eThread.h"
#endif

#ifdef OXYGINE_SDL
#include "SDL_thread.h"
#endif

namespace oxygine
{

#if OXYGINE_SDL
    typedef int s3eThread;
    void* s3eThreadCreate(void* (*func)(void*), void* data)
    {
        return SDL_CreateThread((SDL_ThreadFunction)func, "loading", data);
    }

    void s3eThreadJoin(void* t)
    {
        int s = 0;
        SDL_WaitThread((SDL_Thread*)t, &s);
    }
#elif __S3E__
#else
    typedef int s3eThread;
    void* s3eThreadCreate(void* (*func)(void*), void* data)
    {
        log::warning("not implemented");
        return 0;
    }

    void s3eThreadJoin(void* t)
    {
        log::warning("not implemented");
    }
#endif

    class Mem2Native;
    class LoadResourcesContextMT: public LoadResourcesContext
    {
    public:
        LoadResourcesContextMT(Mem2Native* m2n): _m2n(m2n) {}
        ~LoadResourcesContextMT() {}

        void createTexture(spMemoryTexture src, spNativeTexture dest)
        {
            _m2n->push(src, dest);
        }

        bool isNeedProceed(spNativeTexture t)
        {
            return t->getHandle() == 0;
        }

    private:
        Mem2Native* _m2n;
    };

    void ThreadLoading::copyFrom(const ThreadLoading& src, cloneOptions opt)
    {
        Actor::copyFrom(src, opt);
        _thread = pthread_self();
        _threadDone = false;

        _resources = src._resources;
        _ress = src._ress;
    }

    ThreadLoading::ThreadLoading(): _thread(pthread_self()), _threadDone(false)
    {
        setTouchEnabled(false);
    }

    ThreadLoading::~ThreadLoading()
    {
        //if (!pthread_equal(_thread, pthread_self()))
        //  pthread_join(_thread, 0);
    }

    /*
    float ThreadLoading::getProgress() const
    {
        return 0;
    }
    */

    void ThreadLoading::add(Resources* res)
    {
        _resources.push_back(res);
    }

    void ThreadLoading::add(Resource* res)
    {
        _ress.push_back(res);
    }

    bool ThreadLoading::isCompleted()
    {
        MutexAutoLock k(_m);
        if (_threadDone && _m2n.isEmpty())
            return true;

        return false;
    }

    void* ThreadLoading::_staticThreadFunc(void* t)
    {
        ThreadLoading* This = (ThreadLoading*)t;
        LoadResourcesContextMT ctx(&This->_m2n);
        This->_threadFunc(&ctx);
        return 0;
    }

    void ThreadLoading::load()
    {
        _threadFunc(0);
    }

    void ThreadLoading::unload()
    {
        for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
        {
            Resources* res = *i;
            res->unload();
        }

        for (ress::iterator i = _ress.begin(); i != _ress.end(); ++i)
        {
            Resource* res = *i;
            res->unload();
        }
    }

    void ThreadLoading::_threadFunc(LoadResourcesContextMT* ctx)
    {
        for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
        {
            Resources* res = *i;
            res->load(ctx);
        }

        for (ress::iterator i = _ress.begin(); i != _ress.end(); ++i)
        {
            Resource* res = *i;
            //log::messageln("loading res: %s", res->getName().c_str());
            res->load(ctx);
        }

        MutexAutoLock k(_m);
        _threadDone = true;
    }

    void ThreadLoading::doUpdate(const UpdateState& us)
    {
        _m2n.update();

        if (isCompleted())
        {
            Event ev(COMPLETE, true);

            dispatchEvent(&ev);

            detach();
        }
    }

    void ThreadLoading::setUpdateSize(int s)
    {
        _m2n.setUpdateSize(s);
    }

    void ThreadLoading::start(spActor parent)
    {
        {
            MutexAutoLock k(_m);
            _threadDone = false;
        }

        parent->addChild(this);
        pthread_create(&_thread, 0, _staticThreadFunc, this);
        //_thread = s3eThreadCreate(_staticThreadFunc, this);
    }

    /*
    void ThreadLoading::stop()
    {

    }
    */
}