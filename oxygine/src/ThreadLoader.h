#pragma once
#include "oxygine_include.h"
#include "EventDispatcher.h"
#include "pthread.h"
#include "core/ThreadMessages.h"

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

        void start();
        //void stop();

        /**Loads resources from current thread WITHOUT creating new (for debug)*/
        void load();
        void unload();

    private:
        static void* _staticThreadFunc(void* t);
        virtual void _threadFunc();

        pthread_t _thread;
        volatile bool _threadDone;

        typedef std::list<Resources*> resources;
        resources _resources;

        typedef std::list<Resource*> ress;
        ress _ress;
    };
}