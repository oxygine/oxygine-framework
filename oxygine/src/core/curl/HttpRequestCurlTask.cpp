#include "HttpRequestCurlTask.h"
#include "core/oxygine.h"
#include "core/ThreadDispatcher.h"
#include "SDL.h"
#include "pthread.h"
#include "key.h"

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
                    task->onComplete();
                else
                    task->onError();

                task->releaseRef();
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

#ifdef OX_DEBUG___
                    if (fdread.fd_count > 0)
                    {
                        if (key::isPressed(SDL_SCANCODE_P))
                            closesocket(fdread.fd_array[0]);
                    }
#endif

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


    size_t HttpRequestTaskCURL::cbWriteFunction(char* d, size_t n, size_t l, HttpRequestTaskCURL* This)
    {
        return This->_cbWriteFunction(d, n, l);
    }


    size_t HttpRequestTaskCURL::_cbWriteFunction(char* d, size_t n, size_t l)
    {
        size_t size = n * l;

        write(d, size);

        return size;
    }

    size_t HttpRequestTaskCURL::cbHeaderFunction(char* d, size_t n, size_t l, HttpRequestTaskCURL* This)
    {
        return This->_cbHeaderFunction(d, n, l);
    }


    size_t HttpRequestTaskCURL::_cbHeaderFunction(char* d, size_t n, size_t l)
    {
        size_t s = n * l;
        if (s > 255)//ignore unknown headers
            return s;


        int contentLength = 0;
        if (sscanf(d, "Content-Length: %d", &contentLength) == 1)
        {            
            _expectedContentSize = contentLength;            
        }

        int responseCode = 0;
        char ver[32];
        if (sscanf(d, "HTTP/%s %d ", ver, &responseCode) == 2)
        {
            _responseCode = responseCode;
        }

        if (d[0] == '\r' && d[1] == '\n')
        {
            gotHeaders();
        }

        return s;
    }

    HttpRequestTaskCURL::HttpRequestTaskCURL() : _easy(0), _httpHeaders(0)
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
