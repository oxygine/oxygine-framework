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
#define LOGD(...) 

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

			virtual unsigned int getSize() const
			{
				oxFileSeek(_handle, 0, ox_FILESEEK_END);
				unsigned int size  = oxFileTell(_handle);
				oxFileSeek(_handle, 0, ox_FILESEEK_SET);

				return size;
			}

		private:
			oxHandle* _handle;
		};

		STDFileSystem::STDFileSystem(bool readonly):FileSystem(readonly)
		{
#ifdef WIN32
			if (!_readonly)
				mkdir(getFullPath("").c_str());
#endif
		}

		char* STDFileSystem::_getFullPath(const char *path, char buff[512])
		{
			buff[0] = 0;

			if (_readonly)
			{
#if __S3E__
				sprintf(buff, "rom://%s", path);
#else
				sprintf(buff, "%s%s", _path.c_str(), path);
#endif
			}
			else
			{
#if __S3E__
				sprintf(buff, "ram://%s", path);
#else
	#ifdef  WIN32
				sprintf(buff, "../data-ram/%s%s", _path.c_str(), path);
	#else
				strcpy(buff, path);
				log::warning("not implemented fs");
	#endif
#endif
			}


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
			_path += "/";
			char norm[512];
			path::normalize(_path.c_str(), norm);
			_path = norm;
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
			::remove(buff);
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