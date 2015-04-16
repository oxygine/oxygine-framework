#include "MyHttp.h"
//#include "net/downloader.h"
//#include "shared.h"
//#include "utils/debug_tools.h"
//#include "utils/net_utils.h"

#define NETWORK_AVAILABLE() OX_ASSERT(s3eSocketGetInt(S3E_SOCKET_NETWORK_AVAILABLE) == 1)



const int httpTimeout = 20000;

const int bufSize = 1024 * 64;

MyHttp::MyHttp() : _http(0), _status(status_none), _handle(0), _method(GETFILE)
{

}

MyHttp::~MyHttp()
{
    destroy();
}

void MyHttp::destroy()
{
    if (_http)
        _http->Cancel();
    delete _http;
    _http = 0;
    if (_handle)
        file::close(_handle);
    _handle = 0;
    _status = status_none;
    _data.clear();
    _tempBuffer.clear();
}

unsigned int MyHttp::getTotalSize() const
{
    return _http->ContentLength();
}

unsigned int MyHttp::getReceivedSize() const
{
    return _http->ContentReceived();
}

int MyHttp::_delayedError(void* systemData, void* userData)
{
    MyHttp* http = (MyHttp*)userData;
    http->onError();
    return 0;
}

void MyHttp::getFile(const string& url, const string& name)
{
    destroy();

    _method = GETFILE;
    _url = url;

    log::messageln("getfile:");
    puts(url.c_str());

    _status = status_inprogress;
    _http = new CIwHTTP;

    if (!name.empty())
        _handle = file::open(name.c_str(), "wb");

    _http->Get(url.c_str(), _gotHeaders, this);
}

void MyHttp::get(const string& url)
{
    destroy();

    _method = GET;
    _url = url;
    log::messageln("post: %s", url.c_str());
    _status = status_inprogress;


    _http = new CIwHTTP;
    //_http->SetRequestHeader("Content-type", "application/x-www-form-urlencoded");

    if (!isNetworkAvaible())
    {
        //it is too dangerous call onError from there
        //do it at next update
#ifdef __S3E__
        s3eThreadEnqueueCallback(s3eThreadGetCurrent(), _delayedError, this);
#endif
        return;
    }


    _http->Get(url.c_str(), _gotHeaders, this);
}

void MyHttp::post(const string& url, const char* data, int size)
{
    destroy();

    _post = data;
    _postSize = size;
    _method = POST;
    _url = url;
    log::messageln("post: %s", url.c_str());
    _status = status_inprogress;


    _http = new CIwHTTP;
    _http->SetRequestHeader("Content-type", "application/x-www-form-urlencoded");

    if (!isNetworkAvaible())
    {
        //it is too dangerous call onError from there
        //do it at next update
#ifdef __S3E__
        s3eThreadEnqueueCallback(s3eThreadGetCurrent(), _delayedError, this);
#endif
        return;
    }


    _http->Post(url.c_str(), data, size, _gotHeaders, this);
}

int MyHttp::getResponseCode() const
{
    return _http->GetResponseCode();
}

void MyHttp::gotHeaders()
{
    //log::messageln("gotHeaders");
    if (_http->GetStatus() == S3E_RESULT_ERROR)
    {
        onError();
    }
    else
    {
        if (_cbProgress)
            _cbProgress(this, 0);

        int resp = _http->GetResponseCode();
        if (resp != 200)
        {
            if (resp == 302)
            {
                string res;
                _http->GetHeader("Location", res);
                if (!res.empty())
                {
                    switch (_method)
                    {
                        case MyHttp::GETFILE:
                            get(res);
                            break;
                        case MyHttp::GET:
                            getFile(res, "");
                            break;
                        case MyHttp::POST:
                            post(res, _post, _postSize);
                            break;
                        default:
                            break;
                    }
                    getFile(res, "");
                    return;
                }
            }

            _status = status_error;
            if (_cbError)
                _cbError(this);
            return;
        }

        int len = _http->ContentExpected();
        if (!len)
            len = 1024;

        if (!_handle)
            _data.reserve(len);

        len = min(bufSize, len);
        _tempBuffer.resize(len);

        _http->ReadDataAsync((char*)&_tempBuffer.front(), len, httpTimeout, _gotData, this);
    }
}

void MyHttp::progress(int size)
{
    if (_cbProgress)
        _cbProgress(this, size);

    if (_handle)
        file::write(_handle, &_tempBuffer.front(), size);
    else
        _data.insert(_data.end(), _tempBuffer.begin(), _tempBuffer.begin() + size);


    int rec = _http->ContentReceived();
    /*
    int32 v = 0;
    _http->GetHeader("Content-Length", v);
    string r;
    _http->GetHeader("Content-Range", r);
    */
    int ln = _http->ContentLength();

    if (!ln)
    {
        //something is wrong
        ln = rec + _http->ContentExpected();
    }

    //if (rec != ln)
    if (!_http->ContentFinished())
    {
        int len = ln - rec;

        len = min(bufSize, len);
        _tempBuffer.resize(len);

        _http->ReadDataAsync((char*)&_tempBuffer.front(), len, httpTimeout, _gotData, this);
        return;
    }

    if (_handle)
        file::close(_handle);
    _handle = 0;
    _status = status_done;
    if (_cbDone)
        _cbDone(this);
}

void MyHttp::onError()
{
    log::messageln("http error: %s", _url.c_str());
    if (_handle)
        file::close(_handle);
    _handle = 0;
    _status = status_error;
    if (_cbError)
        _cbError(this);
}

void MyHttp::gotData(int size)
{
    progress(size);
}

int MyHttp::_gotData(void* systemData, void* userData)
{
    MyHttp* http = (MyHttp*)userData;
    http->gotData((size_t)systemData);
    return 0;
}

int MyHttp::_gotHeaders(void* systemData, void* userData)
{
    MyHttp* http = (MyHttp*)userData;
    http->gotHeaders();
    return 0;
}


class SingleHttpAsyncRequest: public Object
{
public:
    void get(const char* url)
    {
        _http.getFile(url, "");
        _http._cbDone = CLOSURE(this, &SingleHttpAsyncRequest::_delete);
        _http._cbError = CLOSURE(this, &SingleHttpAsyncRequest::_delete);
    }

    void post(const char* url, const char* data, int size)
    {
        _http.post(url, data, size);
        _http._cbDone = CLOSURE(this, &SingleHttpAsyncRequest::_delete);
        _http._cbError = CLOSURE(this, &SingleHttpAsyncRequest::_delete);
    }

    void _delete(MyHttp*)
    {
        delete this;
    }
    MyHttp _http;
};

void makeSingleHttpAsyncGetRequest(const char* url)
{
    if (!isNetworkAvaible())
        return;

    SingleHttpAsyncRequest* r = new SingleHttpAsyncRequest;
    r->get(url);
}

void makeSingleHttpAsyncPostRequest(const char* url, const char* data, int size)
{
    if (!isNetworkAvaible())
        return;

    SingleHttpAsyncRequest* r = new SingleHttpAsyncRequest;
    r->post(url, data, size);
}