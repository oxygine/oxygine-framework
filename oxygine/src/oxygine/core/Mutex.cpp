#include "Mutex.h"
#include "ox_debug.h"
#include "pthread.h"

namespace oxygine
{
    Mutex::Mutex(bool recursive)
    {
#ifndef OX_NO_MT
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
#endif
    }

    Mutex::~Mutex()
    {
#ifndef OX_NO_MT
        pthread_mutex_destroy(&_handle);
#endif
    }

    void Mutex::lock()
    {
#ifndef OX_NO_MT
        pthread_mutex_lock(&_handle);
#endif
    }

    void Mutex::unlock()
    {
#ifndef OX_NO_MT
        pthread_mutex_unlock(&_handle);
#endif
    }
}
