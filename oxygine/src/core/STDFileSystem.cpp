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

		STDFileSystem::STDFileSystem()
		{
#ifdef WIN32
			mkdir("../data-ram");
#endif
		}

		void STDFileSystem::setExtendedFolder(const char *folder)
		{
			_extended = folder;
			_extended += "/";
			char norm[512];
			path::normalize(_extended.c_str(), norm);
			_extended = norm;
		}


		FileSystem::status STDFileSystem::_open(const char *file, const char *mode, error_policy ep, fileHandle*& fh)
		{
			bool write = mode[0] == 'w' || mode[0] == 'W';
			oxHandle* h = 0;
			if (!_extended.empty() && !write)
			{
				string path = _extended + file;
				if (oxExists(path.c_str()))
					h = oxFileOpen(path.c_str(), mode);
			}

			if (!h)
			{
#ifdef WIN32
				char name[255];
				safe_sprintf(name, "../data-ram/%s", file);
				h = oxFileOpen(name, mode);
#endif
			}

			if (!h)
			{
				h = oxFileOpen(file, mode);
			}

			if (!h)
				return status_error;
			
			fh = new fileHandleSTD(h);
			return status_ok;
		}

		bool STDFileSystem::_isExists(const char *file)
		{
#if __S3E__
			string path = _extended + file;
			s3eBool res = s3eFileCheckExists(path.c_str());			
			if (res == S3E_FALSE)
				res = s3eFileCheckExists(file);
			return res == S3E_TRUE;
#else
			handle f = file::open(file, "r", ep_ignore_error);
			autoClose h(f);
			return f != 0;
#endif

		}
	}
}