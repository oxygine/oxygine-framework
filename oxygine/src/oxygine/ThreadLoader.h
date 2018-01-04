#pragma once
#include "oxygine-include.h"
#include "Event.h"
#include "EventDispatcher.h"
#include "pthread.h"
#include "core/ThreadDispatcher.h"
#include <functional>

namespace oxygine
{
    DECLARE_SMART(ThreadLoader, spThreadLoader);
    class ThreadLoader: public EventDispatcher
    {
    public:
        enum
        {
            COMPLETE = Event::COMPLETE,
        };

        ThreadLoader();
        ~ThreadLoader();

        bool isCompleted();

        virtual void add(Resources* res);
        virtual void add(Resource* res);
        virtual void add(const std::function< void() > &f);

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
        void _load();

        pthread_t _thread;
        volatile bool _threadDone;

        typedef std::list<Resources*> resources;
        resources _resources;

        typedef std::list<Resource*> ress;
        ress _ress;
        typedef std::list<std::function<void()> > funcs;
        funcs _funcs;
    };
}