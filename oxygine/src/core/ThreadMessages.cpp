#include "ThreadMessages.h"
#include "log.h"
#include "pthread.h"
namespace oxygine
{
#if 0
#define  LOGDN(...)  log::messageln(__VA_ARGS__)
#define  LOGD(...)  log::message(__VA_ARGS__)
#else
#define  LOGDN(...)  ((void)0)
#define  LOGD(...)  ((void)0)
#endif




    void mywait(pthread_cond_t* cond, pthread_mutex_t* mutex)
    {
#ifdef __S3E__
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        addtime(ts, 300);
        pthread_cond_timedwait(cond, mutex, &ts);
#elif __ANDROID__
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        addtime(ts, 500);
        pthread_cond_timedwait(cond, mutex, &ts);
#else
        pthread_cond_wait(cond, mutex);
#endif
    }


    MutexPthreadLock::MutexPthreadLock(pthread_mutex_t& m, bool lock) : _mutex(m), _locked(lock)
    {
        if (_locked)
            pthread_mutex_lock(&_mutex);
    }

    MutexPthreadLock::~MutexPthreadLock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    ThreadMessages::ThreadMessages(): _id(0), _waitReplyID(0), _result(0)
    {
        pthread_cond_init(&_cond, 0);

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&_mutex, &attr);

        _events.reserve(10);
    }

    ThreadMessages::~ThreadMessages()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
    }

    void ThreadMessages::_waitMessage()
    {
        _replyLast(0);
        while (_events.empty())
            pthread_cond_wait(&_cond, &_mutex);
    }

    void ThreadMessages::wait()
    {
        MutexPthreadLock lock(_mutex);
        _waitMessage();
    }

    void addtime(timespec& ts, int ms)
    {
        ts.tv_nsec += ms * 1000000;
        while (ts.tv_nsec >= 1000000000)
        {
            ts.tv_nsec -= 1000000000;
            ++ts.tv_sec;
        }
    }


    void ThreadMessages::get(message& ev)
    {
        MutexPthreadLock lock(_mutex);
        LOGDN("ThreadMessages::get");

        _waitMessage();

        ev = _events.front();
        _events.erase(_events.begin());
        _last = ev;
        LOGDN("ThreadMessages::get received msgid=%d id=%d", _last.msgid, _last._id);
    }

    bool ThreadMessages::empty()
    {
        MutexPthreadLock lock(_mutex);
        bool v = _events.empty();
        return v;
    }

    size_t ThreadMessages::size()
    {
        MutexPthreadLock lock(_mutex);
        size_t v = _events.size();
        return v;
    }

    void ThreadMessages::clear()
    {
        MutexPthreadLock lock(_mutex);
        _events.clear();
    }

    bool ThreadMessages::peek(peekMessage& ev, bool del)
    {
        if (!ev.num)
            return false;

        bool has = false;

        MutexPthreadLock lock(_mutex);
        if (ev.num == -1)
            ev.num = (int) _events.size();

        _replyLast(0);

        if (!_events.empty() && ev.num > 0)
        {
            static_cast<message&>(ev) = _events.front();
            if (del)
                _events.erase(_events.begin());
            has = true;
            _last = ev;

            ev.num--;
        }

        return has;
    }

    void ThreadMessages::_replyLast(void* val)
    {
        LOGDN("ThreadMessages::_replyLast");

        if (_last.cb)
        {
            LOGDN("ThreadMessages::running callback");
            _last.cb(_last);
            _last.cb = 0;
        }

#ifndef __S3E__
        if (_last.cbFunction)
        {
            LOGDN("ThreadMessages::running callback function");
            _last.cbFunction();
            _last.cbFunction = std::function< void() >();
        }
#endif

        if (_waitReplyID == _last._id)
        {
            LOGDN("ThreadMessages::_replyLast not replied yet");
            _result = val;


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

    void ThreadMessages::reply(void* val)
    {
        MutexPthreadLock lock(_mutex);
        _replyLast(val);
    }


    void ThreadMessages::_waitReply()
    {
        while (_last._id != _waitReplyID)
        {
            //LOGDN("ThreadMessages::send msgid=%d waiting reply...", msgid);
            mywait(&_cond, &_mutex);
        }

        _waitReplyID = 0;// ?
    }

    void* ThreadMessages::send(int msgid, void* arg1, void* arg2)
    {
        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;


        MutexPthreadLock lock(_mutex);
        _pushMessageWithReply(ev);

        LOGDN("ThreadMessages::send msgid=%d pthread_cond_signal.. _waitReplyID=%d", msgid, _waitReplyID);
        _waitReply();

        LOGDN("ThreadMessages::send msgid=%d done", msgid);



        return _result;
    }

    void* ThreadMessages::sendCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData)
    {
        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;
        ev.cb = cb;
        ev.cbData = cbData;

        MutexPthreadLock lock(_mutex);
        _pushMessageWithReply(ev);
        LOGDN("ThreadMessages::sendCallback msgid=%d pthread_cond_signal.. _waitReplyID=%d", msgid, _waitReplyID);

        _waitReply();

        LOGDN("ThreadMessages::sendCallback msgid=%d done", msgid);



        return _result;
    }

    void ThreadMessages::_pushMessageWithReply(message& msg)
    {
        msg._id = ++_id;
        _waitReplyID = msg._id;
        _events.push_back(msg);
        pthread_cond_signal(&_cond);
    }

    void ThreadMessages::_pushMessage(message& msg)
    {
        msg._id = ++_id;
        _events.push_back(msg);
        pthread_cond_signal(&_cond);
    }

    void ThreadMessages::post(int msgid, void* arg1, void* arg2)
    {
        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;

        MutexPthreadLock lock(_mutex);
        _pushMessage(ev);
    }

    void ThreadMessages::postCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData)
    {
        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;
        ev.cb = cb;
        ev.cbData = cbData;

        MutexPthreadLock lock(_mutex);
        _pushMessage(ev);
    }

    void ThreadMessages::removeCallback(int msgid, callback cb, void* cbData)
    {
        MutexPthreadLock lock(_mutex);
        for (messages::iterator i = _events.begin(); i != _events.end(); ++i)
        {
            message& m = *i;
            if (m.cb == cb && m.cbData == cbData && m.msgid == msgid)
            {
                _events.erase(i);
                break;
            }
        }
    }

#ifndef __S3E__
    void ThreadMessages::postCallback(const std::function<void()>& f)
    {
        message ev;
        ev.msgid = 0;
        ev.arg1 = 0;
        ev.arg2 = 0;
        ev.cb = 0;
        ev.cbData = 0;
        ev.cbFunction = f;

        MutexPthreadLock lock(_mutex);
        _pushMessage(ev);
    }
#endif
}