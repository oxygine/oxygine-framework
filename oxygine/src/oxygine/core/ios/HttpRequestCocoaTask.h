//
//  HttpRequestCocoaTask.h
//  oxygine_ios
//
//  Created by Denis on 29/12/14.
//  Copyright (c) 2014 denism. All rights reserved.
//
#pragma once
#include "HttpRequestTask.h"

namespace oxygine
{
    class HttpRequestCocoaTask: public HttpRequestTask
    {
    public:
        HttpRequestCocoaTask();
        ~HttpRequestCocoaTask();

        void complete_(NSData* data, bool error, int respCode);
        void progress_(int loaded, int total);

    protected:
        void _run();

    private:
    };
}

