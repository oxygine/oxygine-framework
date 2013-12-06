#include "ThreadMessages.h"
namespace oxygine
{
	class MutexPthreadLock
	{
	public:
		MutexPthreadLock(pthread_mutex_t &m):_mutex(m)
		{
			pthread_mutex_lock(&_mutex);
		}

		~MutexPthreadLock()
		{
			pthread_mutex_unlock(&_mutex);
		}

		pthread_mutex_t& _mutex;
	};

	ThreadMessages::ThreadMessages():_reply(0), _id(0), _waitReplyID(0), _lastGetID(0), _replied(false)
	{			
		pthread_cond_init(&_cond, 0);
		pthread_mutex_init(&_mutex, 0);	
	}

	ThreadMessages::~ThreadMessages()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_cond);
	}

	void ThreadMessages::wait()
	{
		MutexPthreadLock lock(_mutex);
		while (_events.empty())		
			pthread_cond_wait(&_cond, &_mutex);			
	}

	void ThreadMessages::get(message &ev)
	{
		MutexPthreadLock lock(_mutex);
		if (_lastGetID && _lastGetID == _waitReplyID)
		{
			_replied = true;
			pthread_cond_signal(&_cond);
		}

		while (_events.empty())
			pthread_cond_wait(&_cond, &_mutex);			

		ev = _events.front();
		_events.erase(_events.begin());
		_lastGetID = ev.id;	
	}

	bool ThreadMessages::empty()
	{
		MutexPthreadLock lock(_mutex);
		return _events.empty();
	}

	bool ThreadMessages::peek(message &ev, bool del)
	{
		bool has = false;

		MutexPthreadLock lock(_mutex);
		
		if (_lastGetID && _lastGetID == _waitReplyID)
		{
			_replied = true;
			pthread_cond_signal(&_cond);
		}

		if (!_events.empty())
		{
			ev = _events.front();
			if (del)
				_events.erase(_events.begin());
			has = true;
		}		
		_lastGetID = ev.id;

		return has;
	}

	void ThreadMessages::reply(void *val)
	{		
		MutexPthreadLock lock(_mutex);

		if (_lastGetID == _waitReplyID)
		{
			_replied = true;
			_reply = val;
			pthread_cond_signal(&_cond);
		}		
	}


	void *ThreadMessages::send(void *arg1, void *arg2)
	{
		message ev;
		ev.arg1 = arg1;
		ev.arg2 = arg2;
		ev.id = ++_id;

		MutexPthreadLock lock(_mutex);

		_waitReplyID = ev.id;
		_events.push_back(ev);
		pthread_cond_signal(&_cond);

		void *r = 0;
		while (!_replied)
		{
			pthread_cond_wait(&_cond, &_mutex);
		}
		_waitReplyID = 0;
		_replied = false;
		r = _reply;
		_reply = 0;

		return r;
	}

	void ThreadMessages::post(void *arg1, void *arg2)
	{
		message ev;
		ev.arg1 = arg1;
		ev.arg2 = arg2;
		ev.id = ++_id;

		MutexPthreadLock lock(_mutex);
		_events.push_back(ev);
		pthread_cond_signal(&_cond);
	}
}