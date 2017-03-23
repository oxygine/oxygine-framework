#pragma once
#include "oxygine-include.h"

#if defined(_WIN32) && !defined(__MINGW32__)
typedef struct pthread_mutex_t_* pthread_mutex_t;
#else
#   include "pthread.h"
#endif

namespace oxygine
{
    class Mutex
    {
    public:
        Mutex(bool recursive = false);
        ~Mutex();

        void lock();
        void unlock();

    private:
        Mutex(const Mutex&) {}
        void operator = (const Mutex&) {}

        pthread_mutex_t _handle;
        //void *_handle;
    };

    class MutexAutoLock
    {
    public:
        MutexAutoLock(Mutex& m): _m(m) {_m.lock();}
        ~MutexAutoLock() {_m.unlock();}

    private:
        Mutex& _m;
    };
}
