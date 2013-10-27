#pragma once
#include "oxygine_include.h"
#include <string>
#include "math/Color.h"
#include <stdarg.h>

namespace oxygine
{
	using namespace std;
	namespace path
	{
		/**Splits the pathname path into a pair head + tail, where tail is the last pathname component and head is everything leading up to that.
		example: src = "path/file.ext"  destHead => file.ext,   destTail => "path/"
		*/
		void split(const char *src, char *destHead, char *destTail);

		/**Split the pathname path into a pair, (head, tail) where tail is the last pathname component and head is everything leading up to that.
		example: src = "file.ext"  destHead => file,   destTail => "ext"
		*/
		void splitExt(const char *src, char *destHead, char *destTail);

		void normalize(const char *src, char *dest);
	}

	string lower(const string &str);

	/**ONLY FOR DEBUGGING!!! converts wstring to string. it supports ONLY ANSI*/
	wstring debug_s2ws(const string &str);


	wstring utf8tows(const char *utf8str);
	string	ws2utf8(const wchar_t *wstr);

	/**returns next character and utf8 packed to int*/
	const char* getNextCode(int &code, const char* utf8str);

	/**str - should be int RGB or RGBA hex format*/
	Color	hex2color(const char *str);
	string	color2hex(const Color &);


	//case no sensitive cmp
	int strcmp_cns(const char* nameA, const char* nameB);

	template <size_t N>
	inline
	size_t safe_sprintf( char (&str)[N] , const char *format, ...)
	{
		va_list args;
		va_start(args, format);
		size_t r = vsnprintf(str, N, format, args);
		va_end(args);
		return r;
	}
}