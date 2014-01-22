#include "ThreadMessages.h"
#include "log.h"
namespace oxygine
{
#if 0
#define  LOGDN(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)  log::message(__VA_ARGS__)

#else
#define  LOGDN(...)  ((void)0)
#define  LOGD(...)  ((void)0)
#endif

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

	void mywait(pthread_cond_t *cond, pthread_mutex_t *mutex)
	{
#ifdef __S3E__
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 1;
		//ts.tv_nsec += 5000;
		pthread_cond_timedwait(cond, mutex, &ts);
#else
		pthread_cond_wait(cond, mutex);		
#endif
	}
	
	void ThreadMessages::get(message &ev)
	{		
		MutexPthreadLock lock(_mutex);
		LOGDN("ThreadMessages::get");

		_replyLast(0);

		while (_events.empty())
		{
			LOGDN("ThreadMessages::get pthread_cond_wait");
			mywait(&_cond, &_mutex);
		}

		ev = _events.front();
		_events.erase(_events.begin());
		_last = ev;
		LOGDN("ThreadMessages::get received msgid=%d id=%d", _last.msgid, _last._id);
	}
	/*
	ThreadMessages::messages &ThreadMessages::pause(pthread_mutex_t &mutex)
	{
		mutex = _mutex;
		pthread_mutex_lock(&_mutex);
		return _events;
	}

	void ThreadMessages::resume()
	{
		pthread_mutex_unlock(&_mutex);
	}
	*/

	bool ThreadMessages::empty()
	{
		MutexPthreadLock lock(_mutex);
		return _events.empty();
	}

	void ThreadMessages::clear()
	{
		MutexPthreadLock lock(_mutex);
		_events.resize(0);
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
		LOGDN("ThreadMessages::_replyLast");	
		if (!_last._replied)
		{
			LOGDN("ThreadMessages::_replyLast not replied yet");	
			_last._replied = true;
			_last._result = val;

			if (_last.cb)			
				_last.cb(_last);

			LOGDN("ThreadMessages::_replyLast pre _waitReplyID = %d, _last._id = %d, _last.msgid=%d", _waitReplyID, _last._id, _last.msgid);

			if (_waitReplyID && _last._id == _waitReplyID)
			{
				LOGDN("ThreadMessages::_replyLast pthread_cond_signal _waitReplyID = %d, _last._id = %d, _last.msgid=%d", _waitReplyID, _last._id, _last.msgid);
				pthread_cond_signal(&_cond);
			}
			else
			{
				LOGDN("ThreadMessages::_replyLast else _waitReplyID = %d, _last._id = %d, _last.msgid=%d", _waitReplyID, _last._id, _last.msgid);
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


		MutexPthreadLock lock(_mutex);
		ev._id = ++_id;

		_waitReplyID = ev._id;
		_events.push_back(ev);

		LOGDN("ThreadMessages::send msgid=%d pthread_cond_signal.. _waitReplyID=%d", msgid, _waitReplyID);	
		pthread_cond_signal(&_cond);

		
		if (_last._replied)
		{
			LOGDN("ThreadMessages::send msgid=%d already replied", msgid);	
		}
				
		while (!_last._replied)		
		{
			LOGDN("ThreadMessages::send msgid=%d waiting reply...", msgid);	
			mywait(&_cond, &_mutex);
		}

		LOGDN("ThreadMessages::send msgid=%d done", msgid);	

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

		MutexPthreadLock lock(_mutex);
		ev._id = ++_id;
		_events.push_back(ev);
		pthread_cond_signal(&_cond);
	}

	void ThreadMessages::post(int msgid, void *arg1, void *arg2)
	{
		message ev;
		ev.msgid = msgid;
		ev.arg1 = arg1;
		ev.arg2 = arg2;		

		MutexPthreadLock lock(_mutex);
		ev._id = ++_id;
		_events.push_back(ev);
		pthread_cond_signal(&_cond);
	}
}