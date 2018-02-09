#include "Mutex.h"
#include "ox_debug.h"

#if OX_CPP11THREADS
    #include <mutex>
#else
    #include "pthread.h"
#endif

namespace oxygine
{
    Mutex::Mutex()
    {
#if !OX_CPP11THREADS
        pthread_mutex_init(&_handle, 0);
#endif
    }

    Mutex::~Mutex()
    {
#if !OX_CPP11THREADS
        pthread_mutex_destroy(&_handle);
#endif
    }

    void Mutex::lock()
    {
#if OX_CPP11THREADS
        _mutex.lock();
#else
        pthread_mutex_lock(&_handle);
#endif
    }

    void Mutex::unlock()
    {
#if OX_CPP11THREADS
        _mutex.unlock();
#else
        pthread_mutex_unlock(&_handle);
#endif
    }

    MutexRecursive::MutexRecursive()
    {
#if !OX_CPP11THREADS
        pthread_mutexattr_t   mta;
        pthread_mutexattr_init(&mta);
        pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&_handle, &mta);
#endif
    }

    MutexRecursive::~MutexRecursive()
    {
#if !OX_CPP11THREADS
        pthread_mutex_destroy(&_handle);
#endif
    }

    void MutexRecursive::lock()
    {
#if OX_CPP11THREADS
        _mutex.lock();
#else
        pthread_mutex_lock(&_handle);
#endif
    }

    void MutexRecursive::unlock()
    {
#if OX_CPP11THREADS
        _mutex.unlock();
#else
        pthread_mutex_unlock(&_handle);
#endif
    }
}
