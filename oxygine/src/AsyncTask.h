#pragma once
#include "core/ThreadMessages.h"
#include "EventDispatcher.h"
#include "Event.h"

namespace oxygine
{

#ifdef ERROR
#undef ERROR
#endif

    DECLARE_SMART(AsyncTask, spAsyncTask);

    class AsyncTaskEvent : public Event
    {
    public:
        AsyncTaskEvent(eventType type, AsyncTask* t) : Event(type), task(t) {}
        AsyncTask* task;
    };

    class AsyncTask : public EventDispatcher
    {
    public:
        enum
        {
            ERROR = sysEventID('A', 'T', 'E'),
            PROGRESS = sysEventID('A', 'T', 'P'),
            COMPLETE = sysEventID('A', 'T', 'C')
        };

        enum status
        {
            status_not_started,
            status_inprogress,
            status_completed,
            status_failed
        };


        AsyncTask();
        ~AsyncTask();

        status getStatus() const { return _status; }
        void setStatus(status s) { _status = s; }
        void run();

    protected:
        void onError();
        void onComplete();

        status _status;

        friend class AsyncTaskManager;

        bool _mainThreadSync;
        bool _uiThreadSync;

        virtual void _prerun() {}
        virtual void _run() = 0;
        virtual void _onError() {}
        virtual void _onComplete() {}
        virtual void _onFinal(bool error) {}
        virtual void _onCustom(const ThreadMessages::message&) {};
        virtual void _finalize(bool error) {}

        enum { customID = sysEventID('s', 'c', 's') };
        bool syncEvent(int msgid, void* arg1 = 0, void* arg2 = 0);

    private:

        void _complete();
        void _error();
        void _custom(const ThreadMessages::message& m) { _onCustom(m); }


        static void threadCB(const ThreadMessages::message&);
        void _threadCB(const ThreadMessages::message&);
    };
}