#include "HttpRequestTaskS3E.h"

namespace oxygine
{
    spHttpRequestTask HttpRequestTask::create()
    {
        return new HttpRequestTaskS3E;
    }

    void HttpRequestTask::init()
    {

    }

    void HttpRequestTask::release()
    {

    }
}


HttpRequestTaskS3E::HttpRequestTaskS3E()
{

}

void HttpRequestTaskS3E::_run()
{
    _http._cbDone = CLOSURE(this, &HttpRequestTaskS3E::onDone);
    _http._cbError = CLOSURE(this, &HttpRequestTaskS3E::onError);
    if (_fname.empty())
    {
        if (_postData.empty())
            _http.get(_url);
        else
            _http.post(_url, (const char*)&_postData.front(), _postData.size());
    }
    else
        _http.getFile(_url, _fname);

    addRef();
}

void HttpRequestTaskS3E::onDone(MyHttp*)
{
    std::swap(_response, _http.getBuffer());
    onComplete();
    releaseRef();
}

void HttpRequestTaskS3E::onError(MyHttp*)
{
    AsyncTask::onError();
    releaseRef();
}