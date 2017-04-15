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
    
    HttpRequestTask::HttpRequestTask() : _loaded(0),
        _cacheEnabled(true),
        _continueDownload(false),
        _expectedContentSize(0),
        _receivedContentSize(0),
        _fhandle(0),
        _responseCodeChecker(_defaultChecker200)
    {

    }
    
    HttpRequestTask::~HttpRequestTask()
    {
        log::messageln("~HttpRequestTask");
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

    void HttpRequestTask::_prerun()
    {
        _receivedContentSize = 0;
        _expectedContentSize = 0;
        _responseCode = 0;
        _loaded = 0;
        _response.clear();
        if (_fhandle)
            file::close(_fhandle);
        _fhandle = 0;
        
        if (!_fname.empty())
        {
			const char *mode = _continueDownload ? "ab" : "wb";
			_fhandle = file::open(_fname, mode, ep_ignore_error);
			
			//return;
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
    }

    void HttpRequestTask::dispatchProgress(int loaded, int total)
    {
        int delta = loaded - _loaded;
        _loaded = loaded;
        ProgressEvent event(delta, loaded, total);
        dispatchEvent(&event);
    }

    void HttpRequestTask::_onCustom(const ThreadDispatcher::message& msg)
    {
        dispatchProgress((int)(size_t)msg.arg1, (int)(size_t)msg.arg2);
    }

    void HttpRequestTask::progress(int loaded, int total)
    {
        if (!syncEvent(customID, (void*)(size_t)loaded, (void*)(size_t)total))
        {
            dispatchProgress(loaded, total);
        }
    }

    void HttpRequestTask::_onError()
    {
        log::warning("http request error: %s", _url.c_str());
    }

    void HttpRequestTask::_onComplete()
    {
        log::messageln("http request done (%d): %s", _responseCode, _url.c_str());
    }

    void HttpRequestTask::_dispatchComplete()
    {
        bool ok = _responseCodeChecker(_responseCode);
        
        Event ev(ok ? COMPLETE : ERROR);
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

	void HttpRequestTask::write(const void *data, unsigned int size)
	{
		if (_fhandle)
			file::write(_fhandle, data, size);
		else
		{
			const char *p = (const char*)data;
			_response.insert(_response.end(), p, p + size);
		}
        _receivedContentSize += size;
        progress(_receivedContentSize, _expectedContentSize);
	}

}
