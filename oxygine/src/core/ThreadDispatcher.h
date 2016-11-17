#pragma once
#include "oxygine-include.h"
#include <vector>
#include <functional>

#if defined(_WIN32) && !defined(__MINGW32__)
typedef struct pthread_mutex_t_* pthread_mutex_t;
typedef struct pthread_cond_t_* pthread_cond_t;
#else
#   include "pthread.h"
#endif

namespace oxygine
{
    class MutexPthreadLock
    {
    public:
        MutexPthreadLock(pthread_mutex_t& m, bool lock = true);
        ~MutexPthreadLock();

    protected:
        pthread_mutex_t& _mutex;
        bool _locked;
    };
    /*
    class TDMessage
    {
    public:
        TDMessage& withMSGID(int id) { msgid = id; return *this; }
        TDMessage& withArgs(void* Arg1, void* Arg2 = 0) { arg1 = Arg1; arg2 = Arg2; return *this; }
        TDMessage& withHighPriority() { hp = true; return *this; }
        TDMessage& withReply() { reply = true; return *this; }

        int msgid;
        void* arg1;
        void* arg2;
        bool hp;
        bool reply;

    protected:
    };*/


    /**Messages Queue used for communication between threads*/
    class ThreadDispatcher
    {
    public:
        struct message;
        typedef void (*callback)(const message& m);
        struct message
        {
            message(): msgid(0), arg1(0), arg2(0), cb(0), cbData(0), _id(0), need_reply(false) {}

            int     msgid;
            void*   arg1;
            void*   arg2;

            callback    cb;
            void*       cbData;
#ifndef __S3E__
            std::function< void() >     cbFunction;
#endif

            unsigned int _id;
            bool need_reply;
        };

        struct peekMessage: public message
        {
            peekMessage() : num(-1) {}
            int num;
        };


        ThreadDispatcher();
        ~ThreadDispatcher();

        bool    empty();
        size_t  size();


        //blocking
        void wait();
        //blocking
        void get(message& ev);


        bool peek(peekMessage& ev, bool del);
        void clear();

        //blocking, sends message and waiting reply from other thread
        void* send(int msgid, void* arg1, void* arg2);
        //blocking, sends callback and waiting until it is done
        void* sendCallback(void* arg1, void* arg2, callback cb, void* cbData, bool highPriority = false);
#ifndef __S3E__
        //blocking, sends callback and waiting until it is done
        void sendCallback(const std::function<void()>&);
#endif



        //async, sends post message
        void post(int msgid, void* arg1, void* arg2);
        //async, sends post callback
        void postCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData);
        void postCallback(void* arg1, void* arg2, callback cb, void* cbData);
#ifndef __S3E__
        //async, sends post callback
        void postCallback(const std::function<void()>&);
#endif
        //remove scheduled postCallback
        void removeCallback(int msgid, callback cb, void* cbData);

        void reply(void* val);

        //void post(TDMessage&);
        //void send(TDMessage&);

        std::vector<message>& lockMessages();
        void unlockMessages();

    private:
        void _waitMessage();
        void _waitReply(int id);

        void _runCallbacks();

        void _pushMessage(message&);
        void _pushMessageWaitReply(message&, bool highPriority = false);
        void _popMessage(message&);
        void _popMessageNoCB(message&);
        void _replyLast(void* val);


        pthread_mutex_t _mutex;
        pthread_cond_t _cond;

        typedef std::vector<message> messages;
        messages _events;
        message _last;

        void*   _result;
        unsigned int _id;
        int _replyingTo;
    };

    typedef  ThreadDispatcher ThreadMessages;

    class ThreadDispatcher2
    {
    public:

    protected:
    };
}