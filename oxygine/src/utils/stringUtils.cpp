#include "stringUtils.h"
#include <string.h>
#include <algorithm>
#include "core/oxygine.h"
#include "core/log.h"

#ifdef __S3E__
#include "IwUTF8.h"
#endif

#ifdef OXYGINE_SDL
#include "SDL_stdinc.h"
#endif

namespace oxygine
{
	namespace path
	{
		bool unitTest()
		{
			char dest[255];

			//normalize("//", dest);
			//OX_ASSERT(strcmp(dest, "/") == 0);

			normalize("/../", dest);
			OX_ASSERT(strcmp(dest, "../") == 0);

			normalize("../c\\", dest);
			OX_ASSERT(strcmp(dest, "../c/") == 0);

			normalize("a//", dest);
			OX_ASSERT(strcmp(dest, "a/") == 0);


			normalize("\\/\\///\\/", dest);// \n
			OX_ASSERT(strcmp(dest, "") == 0);

			normalize("a/b/..\\//..///\\/../c\\\\/", dest);
			OX_ASSERT(strcmp(dest, "../c/") == 0);

			normalize("a/../c", dest);
			OX_ASSERT(strcmp(dest, "c") == 0);

			normalize("a/b/../../../c", dest);
			OX_ASSERT(strcmp(dest, "../c") == 0);

			normalize("..a/b/..\\//..///\\/../c\\\\/", dest);
			OX_ASSERT(strcmp(dest, "../c/") == 0);

			return true;
		}

#ifdef OX_DEBUG
		bool b = unitTest();
#endif

		void reverse(char *str)
		{
			int len = strlen(str);
			int half = len/2;
			for (int i = 0; i < half; ++i)
			{
				std::swap(str[i], str[len - i - 1]);
			}
		}

		void split(const char *src, char *destHead, char *destTail)
		{
			OX_ASSERT(src != destHead);
			OX_ASSERT(src != destTail);

			destHead[0] = 0;
			destTail[0] = 0;

			int i = 0;
			int len = strlen(src);
			while (len)
			{
				char c = src[len-1];
				if (c == '\\' || c == '/')
				{
					break;
				}

				destTail[i] = c;
				++i;
				--len;
			}
			destTail[i] = 0;
			reverse(destTail);

			memcpy(destHead, src, len);
			destHead[len] = 0;
		}

		void splitExt(const char *src, char *destHead, char *destTail)
		{
			OX_ASSERT(src != destHead);
			OX_ASSERT(src != destTail);

			destHead[0] = 0;
			destTail[0] = 0;

			int len = strlen(src);
			int i = 0;
			for (i = len - 1; i >= 0; --i)
			{
				if (src[i] == '.')
					break;
			}

			if (i == -1)
				i = len;
			memcpy(destHead, src, i);
			destHead[i] = 0;

			int n = len - i;
			memcpy(destTail, src + i, n);
			destTail[n] = 0;
		}

		void normalize(const char *src, char *dest)
		{
			char *copy = dest;
			OX_ASSERT(src != dest);

			char last = 0;
			

			*dest = 0;
			while(*src)
			{
				char c = *src;
				char last =  dest - 1 >= copy? *(dest - 1) : '/';
				char last_last =  dest - 2 >= copy ? *(dest - 2) : '/';

				if (c == '\\')
					c = '/';
				if (c == '/' && last == '.' && last_last == '.' && dest > copy + 2)
				{
					dest -= 3;
					while (dest > copy)
					{
						--dest;
						if (*dest == '/')
							break;						
					}

					++src;
					continue;
				}
				if (last == c && c == '/')
				{
					++src;
					continue;
				}				

				*dest = c;	

				//*(dest + 1) = 0;
				//log::message(copy);

				++src;
				++dest;
				
			}
			*dest = 0;
			int q=0;
		}
	}

	const char* getNextCode(int &code, const char *utf8str)
	{
		const char *utfstr = utf8str;
		code = 0;
		char *p = (char*)&code;

		const char &c = *utfstr++;
		p[0] = c;

		if((c & 0xC0) == 0xC0)
		{
			p[1] = *utfstr++;
		} else
			if((c & 0xE0) == 0xE0)
			{
				p[1] = *utfstr++;
				p[2] = *utfstr++;
			}

		return utfstr;
	}

	string lower(const string &str)
	{
		std::string data = str; 
		std::transform(data.begin(), data.end(), data.begin(), ::tolower);//todo optimize
		return data;
	}

	wstring debug_s2ws(const string &str)
	{
		std::wstring wstr(str.length(), L' ');
		std::copy(str.begin(), str.end(), wstr.begin());
		return wstr;
	}
	
	wstring utf8tows(const char *utf8str)
	{
		if (!utf8str)
			return L"";

		int n = strlen(utf8str) + 1;
		if (n == 1)
			return L"";

		

#if __S3E__
		wchar_t *s = (wchar_t *)fastAlloc(4 * n);
		s[0] = 0;

		int len = IwUTF8ToWideChar(utf8str, n, (ucs2char*)s, n * 4);
		s[len] = 0;

		wstring str = s;
		fastFree(s);
		
		return str;

#elif OXYGINE_SDL
		wchar_t *s = 0;
		if (sizeof(wchar_t) == 2)		
			s = (wchar_t*)SDL_iconv_string("UCS-2-INTERNAL", "UTF-8", utf8str, n);
		else
			s = (wchar_t*)SDL_iconv_string("UCS-4-INTERNAL", "UTF-8", utf8str, n);

		wstring str = s;
		SDL_free(s);
		return str;
#else
		return L"not_implemented";
#endif
	}

	string	ws2utf8(const wchar_t *wstr)
	{
		if (!wstr)
			return "";

		int n = wcslen(wstr) + 1;
		if (n == 1)
			return "";


#if __S3E__
		char *s = (char *)fastAlloc(4 * n);
		s[0] = 0;

		int len = IwWideCharToUTF8((const ucs2char*)wstr, n, s, n * 4);
		s[len] = 0;

		string str = s;
		fastFree(s);
		return str;
#elif OXYGINE_SDL
		char  *s = 0;
		if (sizeof(wchar_t) == 2)		
			s = SDL_iconv_string("UTF-8", "UCS-2-INTERNAL", (const char*)wstr, n * sizeof(wchar_t));
		else
			s = SDL_iconv_string("UTF-8", "UCS-4-INTERNAL", (const char*)wstr, n * sizeof(wchar_t));

		string str = s;
		SDL_free(s);
		return str;
#else
		return "not_implemented";
#endif		
		return "";		
	}

	Color	hex2color(const char *str)
	{
		int len = strlen(str);
		if (len >= 6)
		{

			int rgba = 0;
			sscanf(str, "%x", &rgba);
			if (len == 6)
				rgba = (rgba << 8) |  0xff;

			return Color(rgba);
		}		

		return Color(0xffffffff);
	}

	string	color2hex(const Color &c)
	{
		char str[255];
		safe_sprintf(str, "%02x%02x%02x%02x", c.r, c.g, c.b, c.a);
		return string(str);
	}

	int strcmp_cns(const char* fileName1, const char* fileName2)
	{
		for (;;)
		{
			char c1 = *(fileName1++);
			char c2 = *(fileName2++);
			if ((c1 >= 'a') && (c1 <= 'z'))
				c1 -= 0x20;
			if ((c2 >= 'a') && (c2 <= 'z'))
				c2 -= 0x20;
			if (c1 == '\0')
				return ((c2 == '\0') ? 0 : -1);
			if (c2 == '\0')
				return 1;
			if (c1 < c2)
				return -1;
			if (c1 > c2)
				return 1;
		}
		return 1;
	}
}