#include <string>
#include "STDFileSystem.h"
#include "oxygine.h"
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
#define	ox_FILESEEK_END S3E_FILESEEK_END
#define	ox_FILESEEK_SET S3E_FILESEEK_SET

#define oxFileRead s3eFileRead
#define oxExists(name) s3eFileCheckExists(name)

#elif OXYGINE_SDL
#ifdef WIN32
#include <direct.h>
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
#define	ox_FILESEEK_END RW_SEEK_END
#define	ox_FILESEEK_SET RW_SEEK_SET

bool sdlExists(const char *name)
{
	//return true;
	LOGD("check file exists: %s___", name);
	oxHandle* h = oxFileOpen_(name, "rb");
	LOGD("check ");
	if (h)
		oxFileClose_(h);
	LOGD("check file exists: %s %d", name, (int)(h != 0));
	return h != 0;
}
#define oxExists(name) sdlExists(name)
#define oxFileRead(ptr, size, n, handle) SDL_RWread(handle, ptr, size, n)

#else

#ifdef WIN32
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
#define	ox_FILESEEK_END SEEK_END
#define	ox_FILESEEK_SET SEEK_SET

#define oxExists(name) (true)
#define oxFileRead(ptr, size, n, handle) fread(ptr, size, n, handle)

#endif//__S3E__


//#define LOGD(...) {}

namespace oxygine
{
	namespace file
	{
		using namespace std;

		int _openedFiles = 0;
		oxHandle* oxFileOpen(const char* filename, const char* mode)
		{
			//log::messageln("oxFileOpen %s", filename);
			oxHandle *h = oxFileOpen_(filename, mode);
			if (h)
				_openedFiles++;
			return h;
		}

		void oxFileClose(oxHandle* file)
		{
			if (file)
				_openedFiles--;
			oxFileClose_(file);
		}


		class STDFileSystem;

		class fileHandleSTD:public fileHandle
		{
		public:
			fileHandleSTD(oxHandle* fh):_handle(fh)
			{
			}

			void release()
			{
				oxFileClose(_handle);
				delete this;
			}

			unsigned int read(void *dest, unsigned int size)
			{
				return oxFileRead(dest, 1, size, _handle);
			}

			unsigned int write(const void *src, unsigned int size)
			{
				return oxFileWrite(src,1, size, _handle);
			}

			int seek(unsigned int offset, int whence)
			{
#ifdef __S3E__
				return oxFileSeek(_handle, offset, (s3eFileSeekOrigin)whence);
#else
				return oxFileSeek(_handle, offset, whence);
#endif
			}

			unsigned int tell() const
			{
				return oxFileTell(_handle);
			}

			virtual unsigned int getSize() const
			{
#ifdef __S3E__
				oxFileSeek(_handle, 0, ox_FILESEEK_END);
				unsigned int size  = (unsigned int)oxFileTell(_handle);
				oxFileSeek(_handle, 0, ox_FILESEEK_SET);
				return size;
#else
				return SDL_RWsize((SDL_RWops*)_handle);				
#endif				
			}

		private:
			oxHandle* _handle;
		};

		STDFileSystem::STDFileSystem(bool readonly):FileSystem(readonly)
		{
		}

		char* STDFileSystem::_getFullPath(const char *path, char buff[512])
		{
			buff[0] = 0;

			sprintf(buff, "%s%s", _path.c_str(), path);

			return buff;
		}

		string STDFileSystem::getFullPath(const char *path)
		{
			char buff[512];
			return _getFullPath(path, buff);
		}

		void STDFileSystem::setPath(const char *folder)
		{
			_path = folder;			
			char norm[512];
			path::normalize(_path.c_str(), norm);			
			_path = norm;
			_path += "/";
		}


		FileSystem::status STDFileSystem::_open(const char *file, const char *mode, error_policy ep, fileHandle*& fh)
		{
			char buff[512];

			oxHandle* h = oxFileOpen(_getFullPath(file, buff), mode);
			
			if (!h)
				return status_error;
			
			fh = new fileHandleSTD(h);
			return status_ok;
		}

		FileSystem::status STDFileSystem::_deleteFile(const char* file)
		{
			char buff[512];
			_getFullPath(file, buff);

#if __S3E__
			s3eFileDelete(buff);
#else
			LOGD("STDFileSystem::_deleteFile %s", buff);
			::remove(buff);
#endif

			return status_error;
		}

		FileSystem::status STDFileSystem::_makeDirectory(const char* path)
		{
			char buff[512];
			_getFullPath(path, buff);

#if __S3E__
			s3eFileMakeDirectory(buff);
#elif WIN32
			_mkdir(buff);
#else
			mkdir(buff, 0777);
#endif

			return status_error;
		}

		FileSystem::status STDFileSystem::_renameFile(const char* src, const char *dest)		
		{
			char buffSrc[512];
			_getFullPath(src, buffSrc);

			char buffDest[512];
			_getFullPath(dest, buffDest);

#if __S3E__
			s3eFileRename(buffSrc, buffDest);
#else
			::rename(buffSrc, buffDest);
#endif

			return status_error;
		}

		bool STDFileSystem::_isExists(const char *file)
		{
			char buff[512];
			_getFullPath(file, buff);
#if __S3E__
			s3eBool res = s3eFileCheckExists(buff);
			return res == S3E_TRUE;
#else
			//todo optimize
			oxHandle *h = oxFileOpen(buff, "r");
			if (h)
				oxFileClose(h);
			return h != 0;
#endif
		}
	}
}