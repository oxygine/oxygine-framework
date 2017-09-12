#include <string>
#include "STDFileSystem.h"
#include "log.h"
#include "utils/stringUtils.h"
#include "Object.h"
#include "file.h"
#ifdef __APPLE__
#include <unistd.h>
#endif



//#define LOGD(...) oxygine::log::messageln(__VA_ARGS__)
#define LOGD(...) ((void)0)

#if __S3E__
#define USE_S3E_FILE
#include "IwAutoFile.h"
#include "s3eFile.h"

typedef s3eFile oxHandle;

#define oxFileOpen_ s3eFileOpen
#define oxFileClose_ s3eFileClose
#define oxFileSeek s3eFileSeek
#define oxFileTell s3eFileTell
#define oxFileWrite s3eFileWrite
#define ox_FILESEEK_END S3E_FILESEEK_END
#define ox_FILESEEK_SET S3E_FILESEEK_SET

#define oxFileRead s3eFileRead
#define oxExists(name) s3eFileCheckExists(name)

#elif OXYGINE_SDL && !OXYGINE_FILESYSTEM_USE_STDIO
#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#endif

#include <sys/stat.h>
#include <sys/types.h>
#include "SDL_rwops.h"

typedef SDL_RWops oxHandle;
//todo remove s3e
#define oxFileOpen_ SDL_RWFromFile
#define oxFileClose_ SDL_RWclose
#define oxFileSeek SDL_RWseek
#define oxFileTell SDL_RWtell
#define oxFileWrite(ptr, size, n, handle) SDL_RWwrite(handle, ptr, size, n)
#define ox_FILESEEK_END RW_SEEK_END
#define ox_FILESEEK_SET RW_SEEK_SET
#define oxFileRead(ptr, size, n, handle) SDL_RWread(handle, ptr, size, n)

#else

#ifdef _WIN32
#include <direct.h>
#endif

#define USE_STDIO
#include <stdio.h>
#include <sys/stat.h>

typedef FILE oxHandle;

//todo remove s3e
#define oxFileOpen_ fopen
#define oxFileClose_ fclose
#define oxFileSeek fseek
#define oxFileTell ftell
#define oxFileWrite fwrite
#define ox_FILESEEK_END SEEK_END
#define ox_FILESEEK_SET SEEK_SET
#define oxFileRead(ptr, size, n, handle) fread(ptr, size, n, handle)

#endif//__S3E__


//#define LOGD(...) {}

namespace oxygine
{
    namespace file
    {
        int _openedFiles = 0;
        oxHandle* oxFileOpen(const char* filename, const char* mode)
        {
            if (*mode == 's')
                ++mode;

            LOGD("oxFileOpen %s", filename);
            oxHandle* h = oxFileOpen_(filename, mode);
            if (h)
                _openedFiles++;
            return h;
        }

        void oxFileClose(oxHandle* file)
        {
            if (file)
            {
                _openedFiles--;
                oxFileClose_(file);
            }
        }


        class STDFileSystem;

        unsigned int oxGetSize(oxHandle* h)
        {

#if OXYGINE_SDL && !OXYGINE_FILESYSTEM_USE_STDIO
            return (int)SDL_RWsize((SDL_RWops*)h);
#else
            oxFileSeek(h, 0, ox_FILESEEK_END);
            unsigned int size = (unsigned int)oxFileTell(h);
            oxFileSeek(h, 0, ox_FILESEEK_SET);
            return size;
#endif
        }

        class fileHandleSTD: public fileHandle
        {
        public:
            fileHandleSTD(FileSystem* fs, oxHandle* fh): fileHandle(fs), _handle(fh)
            {
            }

            void release()
            {
                oxFileClose(_handle);
                delete this;
            }

            unsigned int read(void* dest, unsigned int size)
            {
                return (unsigned int) oxFileRead(dest, 1, size, _handle);
            }

            unsigned int write(const void* src, unsigned int size)
            {
                return (unsigned int) oxFileWrite(src, 1, size, _handle);
            }

            int seek(unsigned int offset, int whence)
            {
#ifdef __S3E__
                return oxFileSeek(_handle, offset, (s3eFileSeekOrigin)whence);
#else
                return (int)oxFileSeek(_handle, offset, whence);
#endif
            }

            unsigned int tell() const
            {
                return (unsigned int)oxFileTell(_handle);
            }

            virtual unsigned int getSize() const
            {
                return oxGetSize(_handle);
            }

        private:
            oxHandle* _handle;
        };



        int remove_directory(const char *path)
        {
#ifndef WIN32
            DIR *d = opendir(path);
            size_t path_len = strlen(path);
            int r = -1;

            if (d)
            {
                struct dirent *p;

                r = 0;

                while (!r && (p = readdir(d)))
                {
                    int r2 = -1;
                    char *buf;
                    size_t len;

                    /* Skip the names "." and ".." as we don't want to recurse on them. */
                    if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                    {
                        continue;
                    }

                    len = path_len + strlen(p->d_name) + 2;
                    buf = malloc(len);

                    if (buf)
                    {
                        struct stat statbuf;

                        snprintf(buf, len, "%s/%s", path, p->d_name);

                        if (!stat(buf, &statbuf))
                        {
                            if (S_ISDIR(statbuf.st_mode))
                            {
                                r2 = remove_directory(buf);
                            }
                            else
                            {
                                r2 = unlink(buf);
                            }
                        }

                        free(buf);
                    }

                    r = r2;
                }

                closedir(d);
            }

            if (!r)
            {
                r = rmdir(path);
            }

            return r;
#else
            // subdirectories have been found
            HANDLE          hFile;                       // Handle to directory
            std::string     root = path;
            std::string     strFilePath;                 // Filepath
            std::string     strPattern;                  // Pattern
            WIN32_FIND_DATA FileInformation;             // File information


            strPattern = root + "\\*.*";
            hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
            if (hFile == INVALID_HANDLE_VALUE)
                return -1;
            
            do
            {
                if (FileInformation.cFileName[0] != '.')
                {
                    strFilePath.erase();
                    strFilePath = root + "\\" + FileInformation.cFileName;

                    if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                    {
                        remove_directory(strFilePath.c_str());
                    }
                    else
                    {
                        DeleteFile(strFilePath.c_str());
                    }
                }
            } while (::FindNextFile(hFile, &FileInformation) == TRUE);

            // Close handle
            ::FindClose(hFile);

            return 0;
#endif
        }




        STDFileSystem::STDFileSystem(bool readonly): FileSystem(readonly)
        {
        }

        char* STDFileSystem::_getFullPath(const char* path, char buff[512])
        {
            buff[0] = 0;

            sprintf(buff, "%s%s", _path.c_str(), path);

            return buff;
        }

        std::string STDFileSystem::getFullPath(const char* path)
        {
            char buff[512];
            return _getFullPath(path, buff);
        }

        void STDFileSystem::setPath(const char* folder)
        {
            _path = folder;
            char norm[512];
            path::normalize(_path.c_str(), norm);
            _path = norm;
            if (!_path.empty() && _path[_path.size() - 1] != '/')
            {
                _path += "/";
            }
        }


        FileSystem::status STDFileSystem::_read(const char* file, file::buffer& dest, error_policy ep)
        {
            char buff[512];

            oxHandle* h = oxFileOpen(_getFullPath(file, buff), "rb");
            if (!h)
                return status_error;
            unsigned int size = oxGetSize(h);
            dest.data.resize(size);
            if (size)
                oxFileRead(&dest.data[0], size, 1, h);

            oxFileClose(h);
            return status_ok;
        }

        FileSystem::status STDFileSystem::_open(const char* file, const char* mode, error_policy ep, fileHandle*& fh)
        {
            char buff[512];

            oxHandle* h = oxFileOpen(_getFullPath(file, buff), mode);

            if (!h)
                return status_error;

            fh = new fileHandleSTD(this, h);
            return status_ok;
        }

        FileSystem::status STDFileSystem::_deleteFile(const char* file)
        {
            char buff[512];
            _getFullPath(file, buff);

#if __S3E__
            return (s3eFileDelete(buff) == S3E_RESULT_SUCCESS ? status_ok : status_error);
#elif OXYGINE_EDITOR
            return status_error;
#elif _WIN32
            std::wstring wsPath = utf8tows(buff);
            return DeleteFileW(wsPath.c_str()) ? status_ok : status_error;
#else
            return (::remove(buff) != -1 ? status_ok : status_error);
#endif

            return status_error;
        }

        FileSystem::status STDFileSystem::_makeDirectory(const char* path)
        {
            char buff[512];
            _getFullPath(path, buff);

#if __S3E__
            return (s3eFileMakeDirectory(buff) == S3E_RESULT_SUCCESS ? status_ok : status_error);
#elif _WIN32
            return (_mkdir(buff) != -1 ? status_ok : status_error);
#else
            return (mkdir(buff, 0777) != -1 ? status_ok : status_error);
#endif
            return status_error;
        }

        FileSystem::status STDFileSystem::_deleteDirectory(const char* path)
        {
            char buff[512];
            _getFullPath(path, buff);           

            return remove_directory(buff) != -1 ? status_ok : status_error;
        }

        FileSystem::status STDFileSystem::_renameFile(const char* src, const char* dest)
        {
            char buffSrc[512];
            _getFullPath(src, buffSrc);

            char buffDest[512];
            _getFullPath(dest, buffDest);

#if __S3E__
            return (s3eFileRename(buffSrc, buffDest) == S3E_RESULT_SUCCESS ? status_ok : status_error);
#elif OXYGINE_EDITOR
            return status_error;
#elif _WIN32
            std::wstring s = utf8tows(buffSrc);
            std::wstring d = utf8tows(buffDest);
            return MoveFileW(s.c_str(), d.c_str()) ? status_ok : status_error;
#else
            return (::rename(buffSrc, buffDest) != -1 ? status_ok : status_error);
#endif
            return status_error;
        }

        bool STDFileSystem::_isExists(const char* file)
        {
            char buff[512];
            _getFullPath(file, buff);
#if __S3E__
            s3eBool res = s3eFileCheckExists(buff);
            return res == S3E_TRUE;
#else
            //todo optimize
            oxHandle* h = oxFileOpen(buff, "rb");
            if (h)
                oxFileClose(h);
            return h != 0;
#endif
        }
    }
}
