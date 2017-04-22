#include "AsyncTask.h"
#include "core/oxygine.h"
#include <typeinfo>
namespace oxygine
{

    AsyncTask::AsyncTask() : _status(status_not_started), _mainThreadSync(false)
    {

    }

    AsyncTask::~AsyncTask()
    {

    }

    void AsyncTask::sync(const std::function< void()>& f)
    {
        addRef();
        core::getMainThreadDispatcher().postCallback([ = ]()
        {
            f();
            releaseRef();
        });
    }

    void AsyncTask::run()
    {
        OX_ASSERT(_status == status_not_started);
        _status = status_inprogress;

        bool ok = _prerun();
        log::messageln("AsyncTask::run %d - %s", getObjectID(), typeid(*this).name());

        sync([ = ]()
        {
            if (ok)
                _run();
            else
                _error();
        });
    }

    void AsyncTask::_complete()
    {
        OX_ASSERT(core::isMainThread());
        log::messageln("AsyncTask::_complete %d - %s", getObjectID(), typeid(*this).name());

        _status = status_completed;
        _onFinal(false);
        _finalize(false);
        _onComplete();

        _dispatchComplete();
    }


    void AsyncTask::_dispatchComplete()
    {
        AsyncTaskEvent ev(COMPLETE, this);
        dispatchEvent(&ev);
    }

    void AsyncTask::_error()
    {
        OX_ASSERT(core::isMainThread());
        log::messageln("AsyncTask::_error %d - %s", getObjectID(), typeid(*this).name());

        _status = status_failed;
        _onFinal(true);
        _finalize(true);
        _onError();

        AsyncTaskEvent ev(ERROR, this);
        dispatchEvent(&ev);
    }

    void AsyncTask::onComplete()
    {
        sync([ = ]()
        {
            _complete();
        });
    }

    void AsyncTask::onError()
    {
        sync([ = ]()
        {
            _error();
        });
    }
}