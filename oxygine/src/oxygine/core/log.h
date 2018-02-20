#pragma once
#include "../oxygine-include.h"
#include <stdarg.h>

namespace oxygine
{
    namespace logs
    {
        /**By default logging is enabled*/
        void enable();
        /**Display logging*/
        void disable();

        typedef void (*error_handler)(const char* txt);
        typedef void (*out_handler)(const char* txt);

        /*run your callback if logs::error was called*/
        void setErrorHandler(error_handler);

        void setOutHandler(out_handler);
        out_handler getOutHandler();

        /**pure out to log*/
        void out(const char* str);

        //outputs message without end line
        void message(const char* format, ...);
        void message_va(const char* format, va_list args);

        //outputs message with end line
        void messageln(const char* format, ...);
        void messageln_va(const char* format, va_list args);


        //outputs message as warning, has end line
        void warning(const char* format, ...);
        void warning_va(const char* format, va_list args);

        //outputs message as error, has end line
        void error(const char* format, ...);
        void error_va(const char* format, va_list args);
    }
}