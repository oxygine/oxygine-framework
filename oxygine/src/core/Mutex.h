#pragma once
#include "oxygine_include.h"
#include "pthread.h"
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
		Mutex(const Mutex &){}
		void operator = (const Mutex &){}

		pthread_mutex_t _handle;
		//void *_handle;
	};

	class MutexAutoLock
	{
	public:
		MutexAutoLock(Mutex &m):_m(m){_m.lock();}
		~MutexAutoLock(){_m.unlock();}

	private:
		Mutex& _m;
	};
}