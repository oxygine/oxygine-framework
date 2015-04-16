#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "core/Mem2Native.h"
#include "res/Resource.h"
#include "pthread.h"

namespace oxygine
{
    DECLARE_SMART(ThreadLoading, spThreadLoading);
    class Resources;
    class Resource;
    class LoadResourcesContextMT;

    class ThreadLoading: public Actor
    {
    public:
        DECLARE_COPYCLONE_NEW(ThreadLoading);

        enum
        {
            COMPLETE = sysEventID('T', 'L', 'C'),
        };

        ThreadLoading();
        ~ThreadLoading();

        //float getProgress() const;

        bool isCompleted();
        virtual void add(Resources* res);
        virtual void add(Resource* res);

        /**Adds self to 'parent' actor as child and starts loading thread. Removes self automatically from 'parent' when loading is completed*/
        void start(spActor parent);
        //void stop();

        /**Loads resources from current thread WITHOUT creating new (for debug)*/
        void load();
        void unload();


        /**Changes size of updated Texture rectangle each frame. Use it only for tests*/
        void setUpdateSize(int s);

    private:
        static void* _staticThreadFunc(void* t);
        virtual void _threadFunc(LoadResourcesContextMT* ctx);
        void doUpdate(const UpdateState& us);

        Mem2Native _m2n;
        Mutex _m;

        pthread_t _thread;
        //void *_thread;
        volatile bool _threadDone;

        typedef std::list<Resources*> resources;
        resources _resources;

        typedef std::list<Resource*> ress;
        ress _ress;
    };
}