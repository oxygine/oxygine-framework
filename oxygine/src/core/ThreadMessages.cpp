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

	ThreadMessages::ThreadMessages():_id(0), _waitReplyID(0)
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
		_replyLast(0);
		while (_events.empty())		
			pthread_cond_wait(&_cond, &_mutex);			
	}

	void ThreadMessages::get(message &ev)
	{
		MutexPthreadLock lock(_mutex);

		_replyLast(0);

		while (_events.empty())
			pthread_cond_wait(&_cond, &_mutex);			

		ev = _events.front();
		_events.erase(_events.begin());
		_last = ev;
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
		_replyLast(0);

		if (!_events.empty())
		{
			ev = _events.front();
			if (del)
				_events.erase(_events.begin());
			has = true;
			_last = ev;
		}		

		return has;
	}

	void ThreadMessages::_replyLast(void *val)
	{
		if (!_last._replied)
		{
			_last._replied = true;
			_last._result = val;

			if (_last.cb)			
				_last.cb(_last);

			if (_waitReplyID && _last._id == _waitReplyID)
			{
				pthread_cond_signal(&_cond);
			}
		}
	}

	void ThreadMessages::reply(void *val)
	{		
		MutexPthreadLock lock(_mutex);
		_replyLast(val);		
	}


	void *ThreadMessages::send(int msgid, void *arg1, void *arg2)
	{
		message ev;
		ev.msgid = msgid;
		ev.arg1 = arg1;
		ev.arg2 = arg2;
		ev._id = ++_id;

		MutexPthreadLock lock(_mutex);

		_waitReplyID = ev._id;
		_events.push_back(ev);
		pthread_cond_signal(&_cond);

		while (!_last._replied)		
			pthread_cond_wait(&_cond, &_mutex);

		_waitReplyID = 0;
		_last._replied = false;

		return _last._result;
	}

	void ThreadMessages::sendCallback(int msgid, void *arg1, void *arg2, callback cb, void *cbData)
	{
		message ev;
		ev.msgid = msgid;
		ev.arg1 = arg1;
		ev.arg2 = arg2;
		ev.cb = cb;
		ev.cbData = cbData;
		ev._id = ++_id;

		MutexPthreadLock lock(_mutex);
		_events.push_back(ev);
		pthread_cond_signal(&_cond);
	}

	void ThreadMessages::post(int msgid, void *arg1, void *arg2)
	{
		message ev;
		ev.msgid = msgid;
		ev.arg1 = arg1;
		ev.arg2 = arg2;
		ev._id = ++_id;

		MutexPthreadLock lock(_mutex);
		_events.push_back(ev);
		pthread_cond_signal(&_cond);
	}
}