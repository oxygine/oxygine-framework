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
            message(): msgid(0), arg1(0), arg2(0), cb(0), cbData(0), _id(0), _result(0), _replied(false) {}

            int     msgid;
            void*   arg1;
            void*   arg2;

            callback    cb;
            void*       cbData;
#ifndef __S3E__
            std::function< void() >     cbFunction;
#endif

            unsigned int _id;
            void*   _result;
            bool    _replied;
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

        void wait();
        void get(message& ev);
        bool peek(peekMessage& ev, bool del);
        void clear();

        void* send(int msgid, void* arg1, void* arg2);
        void post(int msgid, void* arg1, void* arg2);
        void postCallback(int msgid, void* arg1, void* arg2, callback cb, void* cbData);
        void removeCallback(int msgid, callback cb, void* cbData);

#ifndef __S3E__
        void postCallback(const std::function<void()>&);
#endif

        void reply(void* val);

    private:
        void _replyLast(void* val);
        unsigned int _id;
        unsigned int _waitReplyID;
        typedef std::vector<message> messages;
        messages _events;
        message _last;
        pthread_cond_t _cond;
        pthread_mutex_t _mutex;
    };
}