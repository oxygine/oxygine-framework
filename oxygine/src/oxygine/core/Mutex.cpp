#include "Mutex.h"
#include "ox_debug.h"
#include "pthread.h"


namespace oxygine
{
    Mutex::Mutex(bool recursive)
    {
        if (recursive)
        {
            pthread_mutexattr_t   mta;
            pthread_mutexattr_init(&mta);
            pthread_mutexattr_settype(&mta, PTHREAD_MUTEX_RECURSIVE);

            pthread_mutex_init(&_handle, &mta);
        }
        else
        {
            pthread_mutex_init(&_handle, 0);
        }
    }

    Mutex::~Mutex()
    {
        pthread_mutex_destroy(&_handle);
    }

    void Mutex::lock()
    {
        pthread_mutex_lock(&_handle);
    }

    void Mutex::unlock()
    {
        pthread_mutex_unlock(&_handle);
    }
}