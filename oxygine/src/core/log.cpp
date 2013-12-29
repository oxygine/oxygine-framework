#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>




#if defined(ANDROID)
#include <android/log.h>
#define  LOG_TAG    "oxygine"
#define  LOGD(str)  __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, str)

#else
#ifdef WIN32
#include <Windows.h>
#define LOGD(str) fputs(str, stdout); OutputDebugStringA(str);
#else
#define LOGD(str) fputs(str, stdout)
#endif
#endif



namespace oxygine
{
	bool _enabled = true;
	FILE *fh = 0;

	const int SIZE = 512;
	namespace log
	{
		void enable()
		{
			_enabled = true;
		}

		void disable()
		{
			_enabled = false;
		}

		void out(const char *str)
		{
			if (!_enabled)
				return;

			//if (!fh)
			//	fh = fopen("log.txt", "w");
			//fprintf(fh, str);
			//fflush(fh);
			LOGD(str);
		}

		void out_line(char *str, int i)
		{
			out(str);
			out("\n");
		}

		void out_line_prefix(const char *pref, const char* format, va_list args)
		{
			char buff[SIZE] = {0};
			strcpy(buff, pref);
			
			int len = strlen(buff);
			int i = vsnprintf(buff + len, sizeof(buff) - len, format, args);
						
			out_line(buff, i + len);
		}

		void out_prefix(const char *pref, const char* format, va_list args)
		{
			char buff[SIZE] = {0};
			strcpy(buff, pref);

			int len = strlen(buff);
			int i = vsnprintf(buff + len, sizeof(buff) - len, format, args);

			out(buff);
		}

		void message(const char *format, ...)
		{
			va_list args;
			va_start(args, format);
			message_va(format, args);
			va_end(args);
		}

		void message_va(const char *format, va_list args)
		{
			out_prefix("", format, args);
		}

		void warning(const char *format, ...)
		{
			va_list args;
			va_start(args, format);
			warning_va(format, args);
			va_end(args);
		}

		void warning_va(const char *format, va_list args)
		{
			out_line_prefix("warning: ", format, args);
		}

		void error(const char *format, ...)
		{
			va_list args;
			va_start(args, format);
			error_va(format, args);
			va_end(args);
		}

		void error_va(const char *format, va_list args)
		{
			out_line_prefix("error: ", format, args);
		}

		void messageln(const char *format, ...)
		{
			va_list args;
			va_start(args, format);
			messageln_va(format, args);
			va_end(args);
		}

		void messageln_va(const char *format, va_list args)
		{
			out_line_prefix("", format, args);
		}
	}
}