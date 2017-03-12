#include "ThreadDispatcher.h"
#include "log.h"
#include "pthread.h"
#include "Mutex.h"

namespace oxygine
{
#if 0
    static size_t threadID()
    {
        pthread_t pt = pthread_self();
        return ((size_t*)(&pt))[0];
    }
#define  LOGDN(format, ...)  log::messageln("ThreadMessages(%lu)::" format, threadID(), __VA_ARGS__)

#else
#define  LOGDN(...)  ((void)0)

#endif


    MutexPthreadLock::MutexPthreadLock(pthread_mutex_t& m, bool lock) : _mutex(m), _locked(lock)
    {
        if (_locked)
            pthread_mutex_lock(&_mutex);
    }

    MutexPthreadLock::~MutexPthreadLock()
    {
        pthread_mutex_unlock(&_mutex);
    }

    ThreadDispatcher::ThreadDispatcher(): _id(0), _result(0)
    {
#ifndef OX_NO_MT
        pthread_cond_init(&_cond, 0);

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

        pthread_mutex_init(&_mutex, &attr);
#endif
        _events.reserve(10);
    }

    ThreadDispatcher::~ThreadDispatcher()
    {
#ifndef OX_NO_MT
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_cond);
#endif
    }

    void ThreadDispatcher::_waitMessage()
    {
#ifndef OX_NO_MT
        _replyLast(0);

        while (_events.empty())
            pthread_cond_wait(&_cond, &_mutex);
#endif
    }

    void ThreadDispatcher::wait()
    {
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
        _waitMessage();
#endif
    }

    void ThreadDispatcher::get(message& ev)
    {
        {
#ifndef OX_NO_MT
            MutexPthreadLock lock(_mutex);
            LOGDN("get");

#endif
            _waitMessage();

            _last = _events.front();
            _events.erase(_events.begin());
            ev = _last;
        }
        _runCallbacks();
    }


    void ThreadDispatcher::_runCallbacks()
    {
        if (_last.cb)
        {
            LOGDN("running callback for id=%d", _last._id);
            _last.cb(_last);
            _last.cb = 0;
        }


        if (_last.cbFunction)
        {
            LOGDN("running callback function for id=%d", _last._id);
            _last.cbFunction();
            _last.cbFunction = std::function< void() >();
        }
    }


    bool ThreadDispatcher::empty()
    {
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif

        bool v = _events.empty();
        return v;
    }

    size_t ThreadDispatcher::size()
    {
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        size_t v = _events.size();
        return v;
    }

    void ThreadDispatcher::clear()
    {
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _events.clear();
        _last = message();
        _id = 0;
        _result = 0;
        _replyingTo = 0;

    }

    void ThreadDispatcher::_popMessage(message& res)
    {
        _last = _events.front();
        _events.erase(_events.begin());

        LOGDN("_gotMessage id=%d, msgid=%d", _last._id, _last.msgid);
        res = _last;
    }

    bool ThreadDispatcher::peek(peekMessage& ev, bool del)
    {
        if (!ev.num)
            return false;


        bool ret = false;
        {
#ifndef OX_NO_MT
            MutexPthreadLock lock(_mutex);
#endif
            if (ev.num == -1)
                ev.num = (int)_events.size();

            LOGDN("peeking message");

            _replyLast(0);

            if (!_events.empty() && ev.num > 0)
            {
                if (del)
                    _popMessage(ev);
                ev.num--;
                ret = true;
            }
        }

        _runCallbacks();

        return ret;
    }

    void ThreadDispatcher::_replyLast(void* val)
    {
        _replyingTo = _last._id;
        _result = val;

        while (_last.need_reply)
        {
            LOGDN("replying to id=%d", _last._id);

#ifndef OX_NO_MT
            //pthread_cond_signal(&_cond);
            pthread_cond_broadcast(&_cond);
#endif
            pthread_cond_wait(&_cond, &_mutex);
        }
    }



    void ThreadDispatcher::_waitReply(int id)
    {
        do
        {
            LOGDN("ThreadMessages::waiting reply... _replyingTo=%d  myid=%d", _replyingTo, id);
#ifndef OX_NO_MT
            pthread_cond_signal(&_cond);
#endif
            pthread_cond_wait(&_cond, &_mutex);
        }
        while (_replyingTo != id);

        _last.need_reply = false;
#ifndef OX_NO_MT
        pthread_cond_signal(&_cond);
#endif
    }

    void ThreadDispatcher::reply(void* val)
    {
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        OX_ASSERT(_last.need_reply);
        _replyLast(val);
    }


    void* ThreadDispatcher::send(int msgid, void* arg1, void* arg2)
    {
        OX_ASSERT(msgid);

        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;

#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _pushMessageWaitReply(ev);

        return _result;
    }

    void* ThreadDispatcher::sendCallback(void* arg1, void* arg2, callback cb, void* cbData, bool highPriority)
    {
        message ev;
        ev.arg1 = arg1;
        ev.arg2 = arg2;
        ev.cb = cb;
        ev.cbData = cbData;

#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _pushMessageWaitReply(ev, highPriority);

        return _result;
    }

    void ThreadDispatcher::_pushMessageWaitReply(message& msg, bool highPriority)
    {
        msg._id = ++_id;
        msg.need_reply = true;
        LOGDN("_pushMessageWaitReply id=%d msgid=%d", msg._id, msg.msgid);

        if (highPriority)
            _events.insert(_events.begin(), msg);
        else
            _events.push_back(msg);

        _waitReply(msg._id);
        LOGDN("waiting reply  %d - done", msg._id);
    }

    void ThreadDispatcher::_pushMessage(message& msg)
    {
        msg._id = ++_id;
        _events.push_back(msg);
#ifndef OX_NO_MT
        pthread_cond_signal(&_cond);
#endif
    }

    void ThreadDispatcher::post(int msgid, void* arg1, void* arg2)
    {
        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;

#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _pushMessage(ev);
    }

    void ThreadDispatcher::postCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData)
    {
        message ev;
        ev.msgid = msgid;
        ev.arg1 = arg1;
        ev.arg2 = arg2;
        ev.cb = cb;
        ev.cbData = cbData;
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _pushMessage(ev);
    }

    void ThreadDispatcher::postCallback(void* arg1, void* arg2, callback cb, void* cbData)
    {
        message ev;
        ev.arg1 = arg1;
        ev.arg2 = arg2;
        ev.cb = cb;
        ev.cbData = cbData;
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _pushMessage(ev);
    }

    void ThreadDispatcher::removeCallback(int msgid, callback cb, void* cbData)
    {
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
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
    void ThreadDispatcher::postCallback(const std::function<void()>& f)
    {
        message ev;
        ev.cbFunction = f;

#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif

        _pushMessage(ev);
    }

    void ThreadDispatcher::sendCallback(const std::function<void()>& f)
    {
        message ev;
        ev.cbFunction = f;
#ifndef OX_NO_MT
        MutexPthreadLock lock(_mutex);
#endif
        _pushMessageWaitReply(ev);
    }


    std::vector<ThreadDispatcher::message>& ThreadDispatcher::lockMessages()
    {
        pthread_mutex_lock(&_mutex);
        return _events;
    }
    void ThreadDispatcher::unlockMessages()
    {
        pthread_mutex_unlock(&_mutex);
    }
}