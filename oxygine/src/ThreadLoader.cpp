#include "ThreadLoader.h"
#include "Event.h"
#include "res/Resources.h"
#include "res/Resource.h"
#include "core/oxygine.h"

namespace oxygine
{
    ThreadLoader::ThreadLoader(): _thread(pthread_self()), _threadDone(false)
    {
    }

    ThreadLoader::~ThreadLoader()
    {
        if (!pthread_equal(_thread, pthread_self()))
            pthread_join(_thread, 0);
    }


    void ThreadLoader::add(Resources* res)
    {
        _resources.push_back(res);
    }

    void ThreadLoader::add(Resource* res)
    {
        _ress.push_back(res);
    }
#ifndef __S3E__
    void ThreadLoader::add(std::function< void() > v)
    {
        _funcs.push_back(v);
    }
#endif

    bool ThreadLoader::isCompleted()
    {
        return _threadDone;
    }

    void* ThreadLoader::_staticThreadFunc(void* t)
    {
        ThreadLoader* This = (ThreadLoader*)t;
        This->_threadFunc();
        return 0;
    }

    void ThreadLoader::load()
    {
        _threadFunc();
    }

    void ThreadLoader::unload()
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


    void threadDone(const ThreadDispatcher::message& msg)
    {
        ThreadLoader* tl = (ThreadLoader*)msg.cbData;

        Event ev(ThreadLoader::COMPLETE, true);
        tl->dispatchEvent(&ev);
        tl->releaseRef();
    }

    void ThreadLoader::_threadFunc()
    {
        for (resources::iterator i = _resources.begin(); i != _resources.end(); ++i)
        {
            Resources* res = *i;
            res->load();
        }

        for (ress::iterator i = _ress.begin(); i != _ress.end(); ++i)
        {
            Resource* res = *i;
            //log::messageln("loading res: %s", res->getName().c_str());
            res->load();
        }

#ifndef __S3E__
        for (funcs::iterator i = _funcs.begin(); i != _funcs.end(); ++i)
        {
            (*i)();
        }
#endif

        core::getMainThreadDispatcher().postCallback(0, 0, 0, threadDone, this);
    }

    void ThreadLoader::start()
    {
        _threadDone = false;
        addRef();

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_create(&_thread, &attr, _staticThreadFunc, this);
    }
}