#pragma once
#include "oxygine_include.h"
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

        //test
        void get2(message& ev);

        bool peek(peekMessage& ev, bool del);
        void clear();

        //blocking, sends message and waiting reply from other thread
        void* send(int msgid, void* arg1, void* arg2);
        //blocking, sends callback and waiting until it is done
        void sendCallback(void* arg1, void* arg2, callback cb, void* cbData);
#ifndef __S3E__
        //blocking, sends callback and waiting until it is done
        void sendCallback(const std::function<void()>&);
#endif



        //async, sends post message
        void post(int msgid, void* arg1, void* arg2);
        //async, sends post callback
        void postCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData);
#ifndef __S3E__
        //async, sends post callback
        void postCallback(const std::function<void()>&);
#endif
        //remove scheduled postCallback
        void removeCallback(int msgid, callback cb, void* cbData);

        void reply(void* val);

    private:
        void _waitMessage();
        void _waitReply(int id);

        void _pushMessage(message&);
        void _pushMessageWaitReply(message&);
        void _popMessage(message&);
        void _popMessageNoCB(message&);
        void _replyLast(void* val);
        unsigned int _id;
        void*   _result;
        int _replyingTo;

        typedef std::vector<message> messages;
        messages _events;
        message _last;
        pthread_cond_t _cond;
        pthread_mutex_t _mutex;
    };

    typedef  ThreadDispatcher ThreadMessages;
}