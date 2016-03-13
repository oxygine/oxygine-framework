#pragma once
#include "curl/curl.h"
#include "oxygine_include.h"
#include "HttpRequestTask.h"
#include "core/file.h"
#include "core/ThreadDispatcher.h"

namespace oxygine
{
    DECLARE_SMART(HttpRequestTaskCURL, spHttpRequestTaskCURL);
    class HttpRequestTaskCURL : public HttpRequestTask
    {
    public:
        HttpRequestTaskCURL();
        ~HttpRequestTaskCURL();

        void setPostData(const std::vector<char>& data);


    protected:
        void _setFileName(const std::string& name) {}

        friend void* thread(void*);
        friend void mainThreadFunc(const ThreadDispatcher::message& msg);

        static size_t cbWriteFunction(char* d, size_t n, size_t l, void* p);
        size_t _cbWriteFunction(char* d, size_t n, size_t l);

        static size_t cbXRefInfoFunction(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
        size_t _cbXRefInfoFunction(curl_off_t dltotal, curl_off_t dlnow);


        void _run();
        void _finalize(bool error);

        file::handle _handle;
        void* _easy;
    };
}
