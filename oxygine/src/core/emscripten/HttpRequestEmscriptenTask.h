#pragma once
#include "HttpRequestTask.h"

namespace oxygine
{

    class HttpRequestEmscriptenTask: public HttpRequestTask
    {
    public:

    private:
        /*
        typedef void (*em_async_wget2_data_onload_func)(unsigned, void*, void*, unsigned);
        typedef void (*em_async_wget2_data_onerror_func)(unsigned, void*, int, const char*);
        typedef void (*em_async_wget2_data_onprogress_func)(unsigned, void*, int, int);
        */

        static void onload(unsigned, void* This, void* arg1, unsigned arg2){((HttpRequestEmscriptenTask*)This)->_onload(arg1, arg2);}
        static void onerror(unsigned, void* This, int arg1, const char* arg2){((HttpRequestEmscriptenTask*)This)->_onerror(arg1, arg2);}
        static void onprogress(unsigned, void* This, int arg1, int arg2){((HttpRequestEmscriptenTask*)This)->_onprogress(arg1, arg2);}

        void _onload(void*, unsigned);  
        void _onerror(int, const char*);
        void _onprogress(int, int);

        void _run();
    };
}

