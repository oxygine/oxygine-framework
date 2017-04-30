#pragma once
#include "oxygine-include.h"
#include "core/ThreadDispatcher.h"
#include "EventDispatcher.h"
#include "Event.h"
#include "core/oxygine.h"
#ifdef ERROR
#undef ERROR
#endif

namespace oxygine
{
    DECLARE_SMART(AsyncTask, spAsyncTask);

    class AsyncTaskEvent: public Event
    {
    public:
        AsyncTaskEvent(eventType type, AsyncTask* t) : Event(type), task(t) {}
        AsyncTask* task;
    };


    class AsyncTask: public EventDispatcher
    {
    public:
        enum
        {
            ERROR = sysEventID('A', 'T', 'E'),
            PROGRESS = sysEventID('A', 'T', 'P'),
            COMPLETE = Event::COMPLETE,
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


        void sync(const std::function< void()>& f);

        status _status;

        friend class AsyncTaskManager;

        bool _mainThreadSync;

        virtual bool _prerun() { return true; }
        virtual void _run() = 0;
        virtual void _onError() {}
        virtual void _onComplete() {}
        virtual void _onFinal(bool error) {}
        virtual void _finalize(bool error) {}
        virtual void _dispatchComplete();


        template <class F>
        void sync(const F& f, bool addref = true)
        {
            if (_mainThreadSync)
            {
                if (addref)
                {
                    addRef();
                    core::getMainThreadDispatcher().postCallback([ = ]()
                    {
                        f();
                        releaseRef();
                    });
                }
                else
                {
                    core::getMainThreadDispatcher().postCallback(f);
                }

                return;
            }
            f();
        }

    private:

        void _complete();
        void _error();
    };
}
