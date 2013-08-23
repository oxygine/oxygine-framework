#pragma once
#include <stdarg.h>
namespace oxygine
{
	/***/
	namespace log
	{
		void enable();
		void disable();

		void out(const char *str);

		//outputs message without end line
		void message(const char *format, ...);
		void message_va(const char *format, va_list args);

		//outputs message with end line
		void messageln(const char *format, ...);
		void messageln_va(const char *format, va_list args);
		

		//outputs message as warning, has end line
		void warning(const char *format, ...);
		void warning_va(const char *format, va_list args);

		//outputs message as error, has end line
		void error(const char *format, ...);
		void error_va(const char *format, va_list args);
	}
}