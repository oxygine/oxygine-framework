#include "HttpRequestEmscriptenTask.h"
#include <emscripten.h>

namespace oxygine
{
	spHttpRequestTask HttpRequestTask::create()
	{
		return new HttpRequestEmscriptenTask;
	}

	void HttpRequestTask::init()
	{

	}

	void HttpRequestTask::release()
	{
	}

	void HttpRequestEmscriptenTask::_onload(void* data, unsigned size)
	{
		log::messageln("HttpRequestEmscriptenTask::_onload");
		_response.assign((char*)data, (char*)data + size);
		onComplete();
		releaseRef();
	}	

	void HttpRequestEmscriptenTask::_onerror(int, const char*)
	{
		log::messageln("HttpRequestEmscriptenTask::_onerror");
		onError();
		releaseRef();
	}

	void HttpRequestEmscriptenTask::_onprogress(int, int)
	{
		Event ev(HttpRequestTask::PROGRESS);
		dispatchEvent(&ev);
	}

	void HttpRequestEmscriptenTask::_run()
	{
		addRef();

		log::messageln("HttpRequestEmscriptenTask::_run %s", _url.c_str());
		_postData.push_back(0);

		emscripten_async_wget2_data(_url.c_str(), _postData.empty() ? "GET" : "POST", (char*)&_postData.front(), this, false, onload, onerror, onprogress);
	}
	//int  emscripten_async_wget2_data(const char* url, const char* requesttype, const char* param, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress);
	//void emscripten_async_wget2_data(const char* url, const char* requesttype, const char* param, void *arg, int free, void (*onload)(void*, void*, unsigned), void (*onerror)(void*, int, const char*), void (*onprogress)(void*, int, int));
	//void emscripten_async_wget2_data(const char* url, const char* requesttype, const char* param, void *arg, int free, em_async_wget2_data_onload_func onload, em_async_wget2_data_onerror_func onerror, em_async_wget2_data_onprogress_func onprogress);
}