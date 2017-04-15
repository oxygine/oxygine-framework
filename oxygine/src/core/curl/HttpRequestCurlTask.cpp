#include "HttpRequestCurlTask.h"
#include "core/oxygine.h"
#include "core/ThreadDispatcher.h"
#include "SDL.h"
#include "pthread.h"

namespace oxygine
{
    CURLM* multi_handle = 0;
    static pthread_t _thread;

    static ThreadDispatcher _messages;
    //ThreadMessages _main;

    static HttpRequestTask* createCurl()
    {
        return new HttpRequestTaskCURL;
    }

    const unsigned int ID_DONE = sysEventID('C', 'D', 'N');
    const unsigned int ID_PROGRESS = sysEventID('C', 'P', 'R');

    void mainThreadFunc(const ThreadDispatcher::message& msg)
    {
        switch (msg.msgid)
        {
            case ID_DONE:
            {
                CURL* easy = (CURL*)msg.arg1;

                HttpRequestTaskCURL* task = 0;
                curl_easy_getinfo(easy, CURLINFO_PRIVATE, &task);

                bool ok = (size_t)msg.arg2 == CURLE_OK;
                if (ok)
                {
                    int response = 0;
                    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &response);
                    task->_responseCode = response;
                    //ok = response == 200;
                }

#if 0
                const Uint8* data = SDL_GetKeyboardState(0);
                static bool fail = false;

                if (data[SDL_SCANCODE_N])
                    fail = true;
                if (data[SDL_SCANCODE_M])
                    fail = false;

                if (fail)
                    ok = false;
#endif

                if (ok)
                {
                    task->onComplete();
                }
                else
                    task->onError();

                task->releaseRef();
            } break;

            case ID_PROGRESS:
            {
                HttpRequestTaskCURL* task = (HttpRequestTaskCURL*)msg.cbData;
                //task->dispatchProgress((int)(size_t)msg.arg2, (int)(size_t)msg.arg1);
            } break;
        }

    }

    void* thread(void*)
    {
        while (true)
        {
            _messages.wait();

            int still_running = -1;

            while (still_running)
            {
                ThreadDispatcher::peekMessage tmsg;
                if (_messages.peek(tmsg, true))
                {
                    if (tmsg.msgid == 1)
                        return 0;
                    curl_multi_add_handle(multi_handle, (CURL*)tmsg.arg1);
                }

                int prev = still_running;
                curl_multi_perform(multi_handle, &still_running);
                if (still_running)
                {
                    struct timeval timeout;

                    fd_set fdread;
                    fd_set fdwrite;
                    fd_set fdexcep;
                    int maxfd;

                    FD_ZERO(&fdread);
                    FD_ZERO(&fdwrite);
                    FD_ZERO(&fdexcep);

                    /* set a suitable timeout to play around with */
                    timeout.tv_sec = 1;
                    timeout.tv_usec = 0;

                    /* get file descriptors from the transfers */
                    curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);

                    if (maxfd == -1)
                    {
                        sleep(100);
                    }
                    else
                    {
                        int rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
                    }
                }


                if (still_running != prev)
                {
                    CURLMsg* msg = 0;
                    int num;
                    while ((msg = curl_multi_info_read(multi_handle, &num)))
                    {
                        if (msg->msg == CURLMSG_DONE)
                        {
#ifdef OX_HAS_CPP11 //msg broken in VS2010
                            curl_multi_remove_handle(multi_handle, msg->easy_handle);
                            core::getMainThreadDispatcher().postCallback(ID_DONE, msg->easy_handle, (void*)msg->data.result, mainThreadFunc, 0);
#endif
                        }
                    }
                }
            }

        }

        return 0;
    }

    void HttpRequestTask::init()
    {
        if (multi_handle)
            return;
        setCustomRequests(createCurl);
        multi_handle = curl_multi_init();
        pthread_create(&_thread, 0, thread, 0);
    }

    void HttpRequestTask::release()
    {
        _messages.post(1, 0, 0);
        pthread_join(_thread, 0);

        if (multi_handle)
            curl_multi_cleanup(multi_handle);
        multi_handle = 0;
    }

    size_t HttpRequestTaskCURL::cbXRefInfoFunction(void* userData, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        return ((HttpRequestTaskCURL*)userData)->_cbXRefInfoFunction(dltotal, dlnow);
    }

    size_t HttpRequestTaskCURL::_cbXRefInfoFunction(curl_off_t dltotal, curl_off_t dlnow)
    {
        core::getMainThreadDispatcher().postCallback(ID_PROGRESS, (void*)dltotal, (void*)dlnow, mainThreadFunc, this);

        return 0;
    }


    int HttpRequestTaskCURL::cbProgressFunction(void* userData, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        return ((HttpRequestTaskCURL*)userData)->_cbXRefInfoFunction((curl_off_t) dltotal, (curl_off_t)dlnow);
    }

    size_t HttpRequestTaskCURL::cbWriteFunction(char* d, size_t n, size_t l, void* userData)
    {
        return ((HttpRequestTaskCURL*)userData)->_cbWriteFunction(d, n, l);
    }


    size_t HttpRequestTaskCURL::_cbWriteFunction(char* d, size_t n, size_t l)
    {
		size_t size = n * l;
		if (!_continueDownload || _ok)
			write(d, size);

        return size;
    }

	size_t HttpRequestTaskCURL::cbHeaderFunction(char* d, size_t n, size_t l, void* userData)
	{
		return ((HttpRequestTaskCURL*)userData)->_cbHeaderFunction(d, n, l);
	}


	size_t HttpRequestTaskCURL::_cbHeaderFunction(char* d, size_t n, size_t l)
	{
		size_t s = n*l;
		if (_continueDownload && !_ok)
		{
			const char *GOOD1 = "HTTP/1.1 200 ";
			const char *GOOD2 = "HTTP/1.1 206 ";
			if (s >= sizeof(GOOD1) && (
				memcmp(d, GOOD1, sizeof(GOOD1)) == 0 ||
				memcmp(d, GOOD2, sizeof(GOOD2)) == 0))
			{ 
				_ok = true;
			}
		}
		return s;
	}

    HttpRequestTaskCURL::HttpRequestTaskCURL() : _easy(0), _httpHeaders(0), _ok(false)
    {
        _easy = curl_easy_init();
    }

    HttpRequestTaskCURL::~HttpRequestTaskCURL()
    {

        if (_easy)
            curl_easy_cleanup(_easy);
        _easy = 0;

        if (_httpHeaders)
            curl_slist_free_all(_httpHeaders);
    }


    void HttpRequestTaskCURL::_run()
    {
        curl_easy_setopt(_easy, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(_easy, CURLOPT_PRIVATE, this);
		curl_easy_setopt(_easy, CURLOPT_WRITEFUNCTION, HttpRequestTaskCURL::cbWriteFunction);
		curl_easy_setopt(_easy, CURLOPT_WRITEDATA, this);

		curl_easy_setopt(_easy, CURLOPT_HEADERFUNCTION, HttpRequestTaskCURL::cbHeaderFunction);
		curl_easy_setopt(_easy, CURLOPT_HEADERDATA, this);


        curl_easy_setopt(_easy, CURLOPT_NOPROGRESS, 0);

#ifdef CURLOPT_XFERINFOFUNCTION
        curl_easy_setopt(_easy, CURLOPT_XFERINFOFUNCTION, HttpRequestTaskCURL::cbXRefInfoFunction);
        curl_easy_setopt(_easy, CURLOPT_XFERINFODATA, this);
#else

        curl_easy_setopt(_easy, CURLOPT_PROGRESSFUNCTION, HttpRequestTaskCURL::cbProgressFunction);
        curl_easy_setopt(_easy, CURLOPT_PROGRESSDATA, this);
#endif
        curl_easy_setopt(_easy, CURLOPT_FOLLOWLOCATION, true);



        curl_easy_setopt(_easy, CURLOPT_SSL_VERIFYPEER, false);


        if (!_postData.empty())
        {
            curl_easy_setopt(_easy, CURLOPT_POSTFIELDS, &_postData.front());
            curl_easy_setopt(_easy, CURLOPT_POSTFIELDSIZE, _postData.size());
        }

        for (size_t i = 0; i < _headers.size(); ++i)
            _httpHeaders = curl_slist_append(_httpHeaders, (_headers[i].first + ": " + _headers[i].second).c_str());

        curl_easy_setopt(_easy, CURLOPT_HTTPHEADER, _httpHeaders);

        addRef();
        _messages.post(0, _easy, 0);
	}
}
