#pragma once
#include "oxygine_include.h"
#include <string>
#include "math/Color.h"
#include <stdarg.h>

namespace oxygine
{
    namespace path
    {
        /**Splits the pathname path into a pair head + tail, where tail is the last pathname component and head is everything leading up to that.
        example: src = "path/file.ext"  destHead => file.ext,   destTail => "path/"
        */
        void split(const char* src, char* destHead, char* destTail);

        /**Split the pathname path into a pair, (head, tail) where tail is the last pathname component and head is everything leading up to that.
        example: src = "file.ext"  destHead => file,   destTail => "ext"
        */
        void splitExt(const char* src, char* destHead, char* destTail);

        void normalize(const char* src, char* dest);
        std::string normalize(const std::string& pth);

        /**some/file/path.png   ->  path.png*/
        std::string extractFileName(const std::string& path);

        /**
        name.png             ->  name
        some/file/path.png   ->  some/file/path
        */
        std::string extractBaseFileName(const std::string& filename);

        /**some/file/path.png   ->  png*/
        std::string extractFileExt(const std::string& filename);

        /**some/file/path.png   ->  some/file/*/
        std::string extractFolder(const std::string& filename);
    }

    std::string lower(const std::string& str);

    std::wstring    utf8tows(const char* utf8str);
    std::string     ws2utf8(const wchar_t* wstr);

    /**returns next character and utf8 packed to int*/
    const char* getNextCode(int& code, const char* utf8str);

    /**str - should be int RGB or RGBA hex format*/
    Color       hex2color(const char* str);
    std::string color2hex(const Color&);


    //compare 2 strings case no sensitive
    int strcmp_cns(const char* nameA, const char* nameB);

    template <size_t N>
    inline
    size_t safe_sprintf(char (&str)[N] , const char* format, ...)
    {
        va_list args;
        va_start(args, format);
#if defined(_MSC_VER) && !defined(__S3E__)
        size_t r = vsnprintf_s(str, N, format, args);
#else
        size_t r = vsnprintf(str, N, format, args);
#endif
        va_end(args);
        return r;
    }
}