#include "HttpRequestCurlTask.h"
#include "../oxygine.h"
#include "../ThreadDispatcher.h"
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

    const unsigned int ID_FINISH = 3423;

    void* curlThread(void*)
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
                    if (tmsg.msgid == ID_FINISH)
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


                            HttpRequestTaskCURL* task = 0;
                            curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &task);

                            bool ok = msg->data.result == CURLE_OK;
                            if (ok)
                            {
                                int response = 0;
                                curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &response);
                                task->_responseCode = response;
                            }

                            if (ok)
                                task->onComplete();
                            else
                                task->onError();

                            task->releaseRef();
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
        pthread_create(&_thread, 0, curlThread, 0);
    }

    void HttpRequestTask::release()
    {
        _messages.post(ID_FINISH, 0, 0);
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
        progress((int)dlnow, (int)dltotal);

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
        if (!_handle && !_fname.empty())
        {
            _handle = file::open(_fname, "wb");
        }

        size_t size = n * l;
        if (!_fname.empty())
            file::write(_handle, d, (unsigned int)size);
        else
            _response.insert(_response.end(), d, d + size);

        return size;
    }

    HttpRequestTaskCURL::HttpRequestTaskCURL() : _easy(0), _handle(0), _httpHeaders(0)
    {
        _easy = curl_easy_init();
    }

    HttpRequestTaskCURL::~HttpRequestTaskCURL()
    {
        if (_handle)
            file::close(_handle);
        _handle = 0;

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

    void HttpRequestTaskCURL::_finalize(bool error)
    {
        if (_handle)
        {
            file::close(_handle);
            if (error)
                file::deleteFile(_fname);
        }
        _handle = 0;
    }
}
