#include <string>
#include "files_io.h"
#include "oxygine.h"
#include "logging.h"
#include "utils/stringUtils.h"
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

		string _additionalFolder;

		oxHandle *handle2ox(handle h)
		{
			return (oxHandle *)h;
		}

		handle open(const char *file_, const char *mode, error_policy ep)
		{
			//OX_ASSERT(_openedFiles == 0);
			LOGD("open file: %s %s %d", file_, mode, _openedFiles);
			char file[512];
			path::normalize(file_, file);
			LOGD("q1");

			bool write = mode[0] == 'w' || mode[0] == 'W';
			oxHandle *fh = 0;			
			if (!_additionalFolder.empty() && write == false)
			{
				string path = _additionalFolder + file;
				if (oxExists(path.c_str()))
					fh = oxFileOpen(path.c_str(), mode);
			}
			LOGD("q2");
			if (!fh)
			{
				if (write || oxExists(file))
					fh = oxFileOpen(file, mode);
			}			
			LOGD("q3");
			if (!fh)
			{
				handleErrorPolicy(ep, "can't open file: %s", file);
			}

			return fh;
		}

		void close(handle fh)
		{
			LOGD("close file %x", fh);
			OX_ASSERT(fh);
			oxHandle *s3eFH = handle2ox(fh);
			oxFileClose(s3eFH);
		}

		void deleteFile(const char *path)
		{
#if __S3E__
			s3eFileDelete(path);
#else
			log::warning("not implemented");
#endif
		}

		void rename(const char *src, const char *dest)
		{
#if __S3E__
			s3eFileRename(src, dest);
#else
			log::warning("not implemented");
#endif			
		}

		unsigned int read(handle fh, void *dest, unsigned int destSize)
		{			
			OX_ASSERT(fh && dest);
			LOGD("read file %x %d", fh, destSize);

			oxHandle *s3eFH = handle2ox(fh);

			return oxFileRead(dest, 1, destSize, s3eFH);
		}

		void read(const char *file, buffer &dest, error_policy ep)
		{
			dest.data.resize(0);

			handle fh_ = open(file, "rb", ep);
			oxHandle *fh = handle2ox(fh_);

			if (!fh)
			{
				//handleErrorPolicy(ep);
				return;
			}
			read(fh, dest);

			oxFileClose(fh);
		}

		unsigned int read(handle fh_, buffer &dest)
		{
			oxHandle *fh = handle2ox(fh_);
			oxFileSeek(fh, 0, ox_FILESEEK_END);
			unsigned int size  = oxFileTell(fh);
			oxFileSeek(fh, 0, ox_FILESEEK_SET);


			dest.data.resize(size);
			unsigned int t = oxFileRead(&dest.data[0], 1, size, fh);
			LOGD("read file %x %d", fh, t);
			return t;
		}

		void write(handle fh_, const void *data, unsigned int size)
		{
			oxHandle *fh = handle2ox(fh_);

			oxFileWrite(data, 1, size, fh);
		}

		

		bool exists(const char *file)
		{
#if __S3E__
			string path = _additionalFolder + file;
			s3eBool res = s3eFileCheckExists(path.c_str());			
			if (res == S3E_FALSE)
				res = s3eFileCheckExists(file);
			return res == S3E_TRUE;
#else
			handle f = open(file, "r", ep_ignore_error);
			autoClose h(f);
			return f != 0;
#endif
		}

		void makeDirectory(const char *path)
		{
#if __S3E__
			s3eFileMakeDirectory(path);
#elif WIN32
			mkdir(path);
#else
			mkdir(path, 0777);
#endif
		}

		void deleteDirectory(const char *path)
		{
#if __S3E__
			s3eFileDeleteDirectory(path);
#else
			rmdir(path);
#endif
		}

		void write(const char *file, const buffer &data, error_policy ep)
		{
			file::handle h = open(file, "w", ep);
			if (!h)
				return;
			write(h, data.getData(), data.getSize());
			close(h);
		}

		void setExtendedFolder(const char *folder)
		{
			_additionalFolder = folder;
			_additionalFolder += "/";
			char norm[512];
			path::normalize(_additionalFolder.c_str(), norm);
			_additionalFolder = norm;
		}
	}
}