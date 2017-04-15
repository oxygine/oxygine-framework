#pragma once
#include "curl/curl.h"
#include "oxygine-include.h"
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

        static size_t cbWriteFunction(char* d, size_t n, size_t l, HttpRequestTaskCURL* p);
        size_t _cbWriteFunction(char* d, size_t n, size_t l);

        static size_t cbHeaderFunction(char* d, size_t n, size_t l, HttpRequestTaskCURL* p);
        size_t _cbHeaderFunction(char* d, size_t n, size_t l);


        void _run();

        void* _easy;
        curl_slist* _httpHeaders;
    };
}
