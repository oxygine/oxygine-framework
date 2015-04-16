#include "FileSystem.h"
#include "utils/stringUtils.h"
#include <string.h>
#include <algorithm>

namespace oxygine
{
    namespace file
    {
        bool starts_with(const char* string, const char* prefix)
        {
            while (*prefix)
            {
                if (*prefix++ != *string++)
                    return false;
            }

            return true;
        }

        FileSystem::FileSystem(bool readonly): _readonly(readonly)
        {
            _prefix[0] = 0;
        }

        FileSystem::~FileSystem()
        {

        }

        void FileSystem::setPrefix(const char* str)
        {
            strcpy(_prefix, str);
        }

#define FUNC(func, _func, readOnlyCheck, startsWithCheck, args) \
    for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)\
    {\
        FileSystem *fs = *i;\
        if (fs->func args)\
            return status_ok;\
    }\
    if (!starts_with(startsWithCheck, _prefix))\
        return status_error;\
    if (readOnlyCheck && _readonly)\
        return status_error;\
    return _func args;


        bool FileSystem::isExists(const char* file)
        {
            FUNC(isExists, _isExists, false, file, (file));
        }

        FileSystem::status FileSystem::deleteFile(const char* file)
        {
            FUNC(deleteFile, _deleteFile, true, file, (file));
        }

        FileSystem::status FileSystem::renameFile(const char* src, const char* dest)
        {
            FUNC(renameFile, _renameFile, true, src, (src, dest));
        }

        FileSystem::status FileSystem::makeDirectory(const char* dest)
        {
            FUNC(makeDirectory, _makeDirectory, true, dest, (dest));
        }

        FileSystem::status FileSystem::deleteDirectory(const char* dest)
        {
            FUNC(deleteDirectory, _deleteDirectory, true, dest, (dest));
        }

        bool FileSystem::isExistsHere(const char* file)
        {
            char str[512];
            path::normalize(file, str);
            return _isExists(str);
        }

        FileSystem::status FileSystem::open(const char* file, const char* mode, error_policy ep, fileHandle*& fh)
        {
            bool write = mode[0] == 'W' || mode[0] == 'w' || mode[0] == 'A' || mode[0] == 'a';
            FUNC(open, _open, write, file, (file, mode, ep, fh));
        }

        void FileSystem::mount(FileSystem* fs)
        {
            filesystems::iterator i = std::find(_filesystems.begin(), _filesystems.end(), fs);
            OX_ASSERT(i == _filesystems.end());
            _filesystems.push_back(fs);
        }

        void FileSystem::unmount(FileSystem* fs)
        {
            filesystems::iterator i = std::find(_filesystems.begin(), _filesystems.end(), fs);
            if (i != _filesystems.end())
            {
                _filesystems.erase(i);
            }
        }

        bool FileSystem::_isExists(const char* file)
        {
            fileHandle* fh = 0;
            status s = _open(file, "rb", ep_ignore_error, fh);
            if (fh)
                fh->release();
            return s == status_ok;
        }
    }
}