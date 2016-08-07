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

    spHttpRequestTask HttpRequestTask::create()
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
                    ok = response == 200;
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
                    task->onComplete();
                else
                    task->onError();

                task->releaseRef();
            } break;

            case ID_PROGRESS:
            {
                HttpRequestTaskCURL* task = (HttpRequestTaskCURL*)msg.cbData;
                task->dispatchProgress((int)(size_t)msg.arg2, (int)(size_t)msg.arg1);
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
                static int i = 0;
                //log::messageln("upd---------%d - %d", getTimeMS(), ++i);

                ThreadDispatcher::peekMessage tmsg;
                if (_messages.peek(tmsg, true))
                {
                    curl_multi_add_handle(multi_handle, (CURL*)tmsg.arg1);
                    int q = 0;
                }

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

                    /* In a real-world program you OF COURSE check the return code of the
                    function calls, *and* you make sure that maxfd is bigger than -1 so
                    that the call to select() below makes sense! */

                    int rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
                    if (rc == -1)
                        return 0;
                    //while(CURLM_CALL_MULTI_PERFORM == curl_multi_perform(multi_handle, &still_running));
                }


                CURLMsg* msg = 0;
                int num;
                while ((msg = curl_multi_info_read(multi_handle, &num)))
                {
                    if (msg->msg == CURLMSG_DONE)
                    {
                        curl_multi_remove_handle(multi_handle, msg->easy_handle);
                        core::getMainThreadDispatcher().postCallback(ID_DONE, msg->easy_handle, (void*)msg->data.result, mainThreadFunc, 0);
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
        multi_handle = curl_multi_init();
        pthread_create(&_thread, 0, thread, 0);
    }

    void HttpRequestTask::release()
    {
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
        {
            file::write(_handle, d, (unsigned int)size);
        }
        else
        {
            _response.insert(_response.end(), d, d + size);
        }

        return size;
    }

    HttpRequestTaskCURL::HttpRequestTaskCURL() : _easy(0), _handle(0)
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
    }

    void HttpRequestTaskCURL::_run()
    {
        curl_easy_setopt(_easy, CURLOPT_URL, _url.c_str());
        curl_easy_setopt(_easy, CURLOPT_PRIVATE, this);
        curl_easy_setopt(_easy, CURLOPT_WRITEFUNCTION, HttpRequestTaskCURL::cbWriteFunction);
        curl_easy_setopt(_easy, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(_easy, CURLOPT_XFERINFOFUNCTION, HttpRequestTaskCURL::cbXRefInfoFunction);
        curl_easy_setopt(_easy, CURLOPT_XFERINFODATA, this);
        curl_easy_setopt(_easy, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(_easy, CURLOPT_NOPROGRESS, 0);

        if (!_postData.empty())
        {
            curl_slist* headers = NULL; // init to NULL is important
            //headers = curl_slist_append(headers, "Accept:")
            headers = curl_slist_append(headers, "Content-Type: text/plain");
            curl_easy_setopt(_easy, CURLOPT_HTTPHEADER, headers);

            //curl_easy_setopt(_easy, CURLOPT_PORT, 4002);


            //curl_easy_setopt(_easy, CURLOPT_VERBOSE, 1);
            //curl_easy_setopt(_easy, CURLOPT_POST, 1);
            //_sendPostData.push_back(0);
            //char *p = curl_escape(&_postData.front(), _postData.size());
            //log::messageln("data: %s", p);
            //log::messageln("data: %s", &_sendPostData.front());
            //curl_easy_setopt(_easy, CURLOPT_POSTFIELDS, p);
            //curl_easy_setopt(_easy, CURLOPT_TIMEOUT, 9999);
            curl_easy_setopt(_easy, CURLOPT_POSTFIELDS, &_postData.front());
            curl_easy_setopt(_easy, CURLOPT_POSTFIELDSIZE, _postData.size());

            //curl_easy_setopt(_easy, CURLOPT_TCP_KEEPALIVE, 1);
        }

        curl_easy_setopt(_easy, CURLOPT_HEADER, 0);

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
