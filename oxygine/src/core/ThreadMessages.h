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
    class ThreadMessages
    {
    public:
        struct message;
        typedef void (*callback)(const message& m);
        struct message
        {
            message(): msgid(0), arg1(0), arg2(0), cb(0), cbData(0), _id(0) {}

            int     msgid;
            void*   arg1;
            void*   arg2;

            callback    cb;
            void*       cbData;
#ifndef __S3E__
            std::function< void() >     cbFunction;
#endif

            unsigned int _id;
        };

        struct peekMessage: public message
        {
            peekMessage() : num(-1) {}
            int num;
        };


        ThreadMessages();
        ~ThreadMessages();

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
        void sendCallback(void* arg1, void* arg2, callback cb, void* cbData);

        //async, sends post message
        void post(int msgid, void* arg1, void* arg2);
        //async, sends post callback
        void postCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData);

        void removeCallback(int msgid, callback cb, void* cbData);

#ifndef __S3E__
        void postCallback(const std::function<void()>&);
#endif

        void reply(void* val);

    private:
        void _waitMessage();
        void _waitReply();

        void _pushMessage(message&);
        void _pushMessageWaitReply(message&);
        void _gotMessage();
        void _replyLast(void* val);
        unsigned int _id;
        unsigned int _waitReplyID;
        void*   _result;

        typedef std::vector<message> messages;
        messages _events;
        message _last;
        pthread_cond_t _cond;
        pthread_mutex_t _mutex;
    };
}