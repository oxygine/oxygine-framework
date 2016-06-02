#pragma once
#include "oxygine_include.h"
#include "EventDispatcher.h"
#include "pthread.h"
#include "core/ThreadDispatcher.h"
#include <functional>

namespace oxygine
{
    DECLARE_SMART(ThreadLoader, spThreadLoader);
    class Resources;
    class Resource;

    class ThreadLoader: public EventDispatcher
    {
    public:
        enum
        {
            COMPLETE = sysEventID('T', 'L', 'C'),
        };

        ThreadLoader();
        ~ThreadLoader();

        bool isCompleted();

        virtual void add(Resources* res);
        virtual void add(Resource* res);

#ifndef __S3E__
        virtual void add(std::function< void() >);
#endif

        void start();
        //void stop();

        /**Loads resources from current thread WITHOUT creating new (for debug)*/
        void load();
        void unload();

    private:
        static void* _staticThreadFunc(void* t);
		static void threadDone(const ThreadDispatcher::message& msg);
        virtual void _threadFunc();

		void loaded(Event*);

        pthread_t _thread;
        volatile bool _threadDone;

        typedef std::list<Resources*> resources;
        resources _resources;

        typedef std::list<Resource*> ress;
        ress _ress;

#ifndef __S3E__
        typedef std::list<std::function<void()> > funcs;
        funcs _funcs;
#endif
    };
}