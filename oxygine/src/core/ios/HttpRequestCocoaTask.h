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

        void write(NSData* data);
        void complete_(bool error);
        void progress_(int loaded, int total);
        void gotResponse(int resp, size_t expectedSize);

    protected:
        void _run();

    private:
    };
}

