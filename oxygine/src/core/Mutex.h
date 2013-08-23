#pragma once
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

		void *_handle;
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