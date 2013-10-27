#include "Mutex.h"
#include "ox_debug.h"

#if __S3E__
#include "s3eThread.h"
#elif OXYGINE_SDL
#include "SDL_mutex.h"

void *s3eThreadLockCreate()
{
	return SDL_CreateMutex();
}

void s3eThreadLockDestroy(void *m)
{
	SDL_DestroyMutex((SDL_mutex*)m);
}

void s3eThreadLockAcquire(void *m)
{
	SDL_LockMutex((SDL_mutex*)m);
}

void s3eThreadLockRelease(void *m)
{
	SDL_UnlockMutex((SDL_mutex*)m);
}
typedef int s3eThreadLock;
#else
void *s3eThreadLockCreate()
{
	//OX_ASSERT(0);
	return 0;
}

void s3eThreadLockDestroy(void *m)
{
	
}

void s3eThreadLockAcquire(void *m)
{
	
}

void s3eThreadLockRelease(void *m)
{
	
}
typedef int s3eThreadLock;
#endif

namespace oxygine
{
	Mutex::Mutex():_handle(0)
	{
		_handle = s3eThreadLockCreate();
	}

	Mutex::~Mutex()
	{
		s3eThreadLockDestroy((s3eThreadLock*)_handle);
	}

	void Mutex::lock()
	{
		s3eThreadLockAcquire((s3eThreadLock*)_handle);
	}

	void Mutex::unlock()
	{
		s3eThreadLockRelease((s3eThreadLock*)_handle);
	}
}