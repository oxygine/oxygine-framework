#pragma once
#include "oxygine-include.h"

#if OX_CPP11THREADS
    #include <mutex>
#else

#if defined(_WIN32) && !defined(__MINGW32__)
typedef struct pthread_mutex_t_* pthread_mutex_t;
#else
#   include "pthread.h"
#endif

#endif

namespace oxygine
{
    class Mutex
    {
    public:
        Mutex();
        ~Mutex();

        void lock();
        void unlock();

    private:
        Mutex(const Mutex&) {}
        void operator = (const Mutex&) {}

#if OX_CPP11THREADS
        std::mutex _mutex;
#else
        pthread_mutex_t _handle;
        //void *_handle;
#endif
    };

    class MutexRecursive
    {
    public:
        MutexRecursive();
        ~MutexRecursive();

        void lock();
        void unlock();

    private:
        MutexRecursive(const MutexRecursive&) {}
        void operator = (const MutexRecursive&) {}

#if OX_CPP11THREADS
        std::recursive_mutex _mutex;
#else
        pthread_mutex_t _handle;
        //void *_handle;
#endif
    };

    class MutexAutoLock
    {
    public:
        MutexAutoLock(Mutex& m): _m(m) {_m.lock();}
        ~MutexAutoLock() {_m.unlock();}

    private:
        Mutex& _m;
    };

    class MutexRecursiveAutoLock
    {
    public:
        MutexRecursiveAutoLock(MutexRecursive& m): _m(m) {_m.lock();}
        ~MutexRecursiveAutoLock() {_m.unlock();}

    private:
        MutexRecursive& _m;
    };
}
