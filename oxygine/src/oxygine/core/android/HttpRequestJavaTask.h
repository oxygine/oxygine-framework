//
//  HttpRequestCocoaTask.h
//  oxygine_ios
//
//  Created by Denis on 29/12/14.
//  Copyright (c) 2014 denism. All rights reserved.
//
#pragma once
#include "../../oxygine-include.h"
#include "../../HttpRequestTask.h"
#include "jniUtils.h"
#include "jniHelper.h"

namespace oxygine
{
    class HttpRequestJavaTask: public HttpRequestTask
    {
    public:
        HttpRequestJavaTask();
        ~HttpRequestJavaTask();

        void complete_();
        void progress_(int loaded, int total);
        void gotHeader_(int respCode, int contentLen);
        void write_(jbyteArray, int size);
        void error_();


    protected:
        void _run();
        void _finaliaze(bool);
        jobject _handle;
    private:
    };
}

