#pragma once
#include "HttpRequestTask.h"
#include "MyHttp.h"

DECLARE_SMART(HttpRequestTaskS3E, spHttpRequestTaskS3E);
class HttpRequestTaskS3E: public HttpRequestTask
{
public:
    HttpRequestTaskS3E();

protected:
    void _run();

    void onDone(MyHttp*);
    void onError(MyHttp*);
    MyHttp _http;
};

