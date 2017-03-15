#include "Mutex.h"
#include "ox_debug.h"
#include "pthread.h"


namespace oxygine
{
    Mutex::Mutex()//:_handle(PTHREAD_MUTEX_INITIALIZER)
    {
        //_handle = PTHREAD_MUTEX_INITIALIZER;
        pthread_mutex_init(&_handle, 0);
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