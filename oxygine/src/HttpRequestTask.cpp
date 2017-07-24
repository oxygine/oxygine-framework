#include "HttpRequestTask.h"
#include "core/file.h"
#include "core/oxygine.h"
#include "utils/stringUtils.h"

namespace oxygine
{
    HttpRequestTask* createNullHttpTask()
    {
        return 0;
    }

    HttpRequestTask::createHttpRequestCallback _createRequestsCallback = createNullHttpTask;

    spHttpRequestTask HttpRequestTask::create()
    {
        return _createRequestsCallback();
    }

#if OX_NO_HTTP
    void HttpRequestTask::init() {}
    void HttpRequestTask::release() {}
#endif

    static HttpRequestTask::responseCodeChecker _defaultCheckerAny = [](int code)
    {
        return true;
    };


    static HttpRequestTask::responseCodeChecker _defaultChecker200 = [](int code)
    {
        return code == 200 || code == 206;
    };

    HttpRequestTask::HttpRequestTask() :
        _cacheEnabled(true),
        _continueDownload(false),
        _expectedContentSize(0),
        _receivedContentSize(0),
        _fhandle(0),
        _suitableResponse(false),
        _responseCodeChecker(_defaultChecker200)
    {
        setName("HttpRequestTask");

        _mainThreadSync = true;
    }

    HttpRequestTask::~HttpRequestTask()
    {
        if (_fhandle)
            file::close(_fhandle);
    }

    void HttpRequestTask::setCustomRequests(createHttpRequestCallback cb)
    {
        _createRequestsCallback = cb;
    }

    void HttpRequestTask::setPostData(const std::vector<unsigned char>& data)
    {
        _postData = data;
        _setPostData(data);
    }

    void HttpRequestTask::setUrl(const std::string& url)
    {
        _url = url;
        _setUrl(url);
    }

    void HttpRequestTask::setFileName(const std::string& name, bool continueDownload)
    {
        _continueDownload = continueDownload;
        _fname = name;
        _setFileName(name);
    }

    void HttpRequestTask::setCacheEnabled(bool enabled)
    {
        _cacheEnabled = enabled;
        _setCacheEnabled(enabled);
    }

    void HttpRequestTask::setSuccessOnAnyResponseCode(bool any)
    {
        _responseCodeChecker = any ? _defaultCheckerAny : _defaultChecker200;
    }

    void HttpRequestTask::addHeader(const std::string& key, const std::string& value)
    {
        OX_ASSERT(!key.empty());

        _headers.push_back(std::make_pair(key, value));
        _addHeader(key, value);
    }

    const std::vector<unsigned char>&   HttpRequestTask::getPostData() const
    {
        return _postData;
    }

    const std::vector<unsigned char>& HttpRequestTask::getResponse() const
    {
        return _response;
    }

    void HttpRequestTask::getResponseSwap(std::vector<unsigned char>& r)
    {
        std::swap(r, _response);
    }

    const std::string& HttpRequestTask::getFileName() const
    {
        return _fname;
    }

    const std::string& HttpRequestTask::getUrl() const
    {
        return _url;
    }

    bool HttpRequestTask::_prerun()
    {
        _progressDeltaDelayed = 0;
        _progressDispatched = false;
        _suitableResponse = false;
        _receivedContentSize = 0;
        _expectedContentSize = 0;
        _responseCode = 0;
        _response.clear();
        _writeFileError = false;
        if (_fhandle)
            file::close(_fhandle);
        _fhandle = 0;

        if (!_fname.empty())
        {
            const char* mode = _continueDownload ? "ab" : "wb";
            _fhandle = file::open(_fname, mode);
            OX_ASSERT(_fhandle);

            if (!_fhandle)
            {
                return false;
            }

            if (_continueDownload)
            {
                file::seek(_fhandle, 0, SEEK_END);
                unsigned int size = file::tell(_fhandle);

                char str[255];
                safe_sprintf(str, "bytes=%d-", size);
                addHeader("Range", str);

                _receivedContentSize = size;
            }
        }
        return true;
    }

    void HttpRequestTask::dispatchProgress(size_t delta, size_t loaded, size_t total)
    {
        ProgressEvent event(delta, loaded, total);
        dispatchEvent(&event);
    }

    void HttpRequestTask::asyncProgress(size_t delta, size_t loaded, size_t total)
    {
        if (_progressDispatched && loaded != total)//dispatch progress only once per frame
        {
            _progressDeltaDelayed += delta;
            return;
        }

        _progressDispatched = true;
        sync([ = ]()
        {
            _progressDispatched = false;
            dispatchProgress(delta + _progressDeltaDelayed, loaded, total);
            _progressDeltaDelayed = 0;
        }, false);
    }

    void HttpRequestTask::_onError()
    {
        log::warning("http request error (%d): %s", _responseCode , _url.c_str());
    }

    void HttpRequestTask::_onComplete()
    {
        log::messageln("http request done (%d): %s", _responseCode, _url.c_str());
    }

    void HttpRequestTask::_dispatchComplete()
    {
        unsigned int id = _suitableResponse ? COMPLETE : ERROR;
        if (_writeFileError)
            id = ERROR;
        Event ev(id);
        dispatchEvent(&ev);
    }

    void HttpRequestTask::_finalize(bool error)
    {
        if (_fhandle)
        {
            file::close(_fhandle);
            _fhandle = 0;

            if (error && !_continueDownload)
                file::deleteFile(_fname);
        }
        _fhandle = 0;
    }

    void HttpRequestTask::gotHeaders()
    {
        _suitableResponse = _responseCodeChecker(_responseCode);

        if (_continueDownload)
            asyncProgress(_receivedContentSize, _receivedContentSize, _expectedContentSize);
    }

    void HttpRequestTask::write(const void* data, size_t size)
    {
        if (!_suitableResponse)
            return;


        if (_fhandle)
        {
            unsigned int written = file::write(_fhandle, data, (unsigned int)size);
            if (written != size)
            {
                log::messageln("WRITE FILE ERROR %d %d", written, size);
                _writeFileError = true;
                return;
            }
        }
        else
        {
            const char* p = (const char*)data;
            _response.insert(_response.end(), p, p + size);
        }

        _receivedContentSize += size;
        asyncProgress(size, _receivedContentSize, _expectedContentSize);
    }

}
