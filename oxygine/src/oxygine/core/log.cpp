#include "log.h"
#include "../core/oxygine.h"
#include "../utils/stringUtils.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if defined(ANDROID)
#include <android/log.h>
#define  LOG_TAG    "SDL"
#define  LOGD(str)  __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, str)

#else
#ifdef _WIN32
#include <Windows.h>
#define LOGD(str) fputs(str, stdout); OutputDebugStringA(str);
#else
#define LOGD(str) fputs(str, stdout)
#endif
#endif


namespace oxygine
{
    bool _enabled = true;
    FILE* fh = 0;



#ifdef OX_DEBUG
    const int SIZE = 16384;
#else
    const int SIZE = 4096;
#endif

    namespace logs
    {
        int getTime()
        {
            static int64 startTime = getTimeUTCMS();
            return (int)(getTimeUTCMS() - startTime);
        }

        void enable()
        {
            _enabled = true;
            messageln("***log enabled***");
        }

        void disable()
        {
            messageln("***log disabled***");
            _enabled = false;
        }

        void outDefault(const char* txt)
        {
            LOGD(txt);
        }


        error_handler _eh = 0;
        out_handler _oh = outDefault;

        void setErrorHandler(error_handler eh)
        {
            _eh = eh;
        }

        void setOutHandler(out_handler h)
        {
            _oh = h;
        }

        out_handler getOutHandler()
        {
            return _oh;
        }

        void out(const char* str)
        {
            if (!_enabled)
                return;

            _oh(str);
        }

        void out_line(char* str, int i)
        {
            out(str);
#if __ANDROID__
#else
            out("\n");
#endif
        }

        void out_line_prefix(error_handler eh, const char* pref, const char* format, va_list args)
        {
            char buff[SIZE] = {0};
            strcpy(buff, pref);

            int len = (int)strlen(buff);
            int i = vsnprintf(buff + len, sizeof(buff) - len, format, args);
            if (i == -1)
                buff[SIZE - 1] = 0;
            out_line(buff, i + len);

            if (eh)
                eh(buff);
        }

        void out_prefix(error_handler eh, const char* pref, const char* format, va_list args)
        {
            char buff[SIZE] = {0};
            strcpy(buff, pref);

            int len = (int)strlen(buff);
            int i = vsnprintf(buff + len, sizeof(buff) - len, format, args);
            if (i == -1)
                buff[SIZE - 1] = 0;
            out(buff);

            if (eh)
                eh(buff);
        }

        void message(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            message_va(format, args);
            va_end(args);
        }

        void message_va(const char* format, va_list args)
        {
            char tm[16];
            safe_sprintf(tm, "%03d", getTime());
            out_prefix(0, tm, format, args);
        }

        void warning(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            warning_va(format, args);
            va_end(args);
        }

        void warning_va(const char* format, va_list args)
        {
            char tm[32];
            safe_sprintf(tm, "%03d warning: ", getTime());

            out_line_prefix(0, tm, format, args);
        }

        void error(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            error_va(format, args);
            va_end(args);
#ifdef EMSCRIPTEN
            emscStackTrace();
#endif
        }

        void error_va(const char* format, va_list args)
        {
            char tm[32];
            safe_sprintf(tm, "%03d error: ", getTime());
            out_line_prefix(_eh, tm, format, args);
        }

        void messageln(const char* format, ...)
        {
            va_list args;
            va_start(args, format);
            messageln_va(format, args);
            va_end(args);
        }

        void messageln_va(const char* format, va_list args)
        {
            char tm[16];
            safe_sprintf(tm, "%03d ", getTime());
            out_line_prefix(0, tm, format, args);
        }
    }
}