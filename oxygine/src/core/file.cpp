#include <string>
#include "STDFileSystem.h"
#include "file.h"
#include "oxygine.h"
#include "log.h"
#include "utils/stringUtils.h"
#include "Object.h"

#if defined(__APPLE__) || defined(ANDROID) || defined(__unix__)
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef __S3E__
#include <s3eFile.h>
#else
#if WIN32
#include <direct.h>
#else
#include "SDL_system.h"
#endif
#endif



//#define LOGD(...) oxygine::log::messageln(__VA_ARGS__)
#define LOGD(...) 


namespace oxygine
{
	namespace file
	{
		string _additionalFolder;

		STDFileSystem _nfs(true);
		STDFileSystem _nfsWrite(false);

		void init()
		{
#ifdef __S3E__
			_nfs.setPath("rom://");
			_nfsWrite.setPath("ram://");
#endif

#ifdef __ANDROID__
			log::messageln("internal %s", SDL_AndroidGetInternalStoragePath());
			log::messageln("external %s", SDL_AndroidGetExternalStoragePath());
			_nfsWrite.setPath(SDL_AndroidGetInternalStoragePath());
#endif // __ANDROID__

#ifdef WIN32
			_mkdir("../data-ram/");
			_nfsWrite.setPath("../data-ram/");			
#endif

			_nfs.mount(&_nfsWrite);
		}

		void mount(FileSystem *fs)
		{
			_nfs.mount(fs);
		}

		void unmount(FileSystem *fs)
		{
			_nfs.unmount(fs);
		}

		handle open(const char *file_, const char *mode, error_policy ep)
		{
			//OX_ASSERT(_openedFiles == 0);
			LOGD("open file: %s %s %d", file_, mode, _openedFiles);
			char file[512];
			path::normalize(file_, file);
			LOGD("q1");


			fileHandle *fh = 0;
			_nfs.open(file, mode, ep_ignore_error, fh);

			LOGD("q3");

			if (!fh)
			{
				handleErrorPolicy(ep, "can't open file: %s", file);
			}

			return fh;
		}

		void close(handle h)
		{
			LOGD("close file %x", fh);
			fileHandle *fh = (fileHandle *)h;
			fh->release();
		}

		int seek(handle h, unsigned int offset, int whence)
		{
			fileHandle *fh = (fileHandle *)h;
			return fh->seek(offset, whence);
		}

		unsigned int tell(handle h)
		{
			fileHandle *fh = (fileHandle *)h;
			return fh->tell();
		}

		bool deleteFile(const char *path, error_policy ep)
		{
			bool ok = _nfs.deleteFile(path) == FileSystem::status_ok;
			if (!ok)
			{
				handleErrorPolicy(ep, "can't delete file: %s", path);
			}

			return ok;
		}

		bool rename(const char *src, const char *dest, error_policy ep)
		{
			bool ok = _nfs.renameFile(src, dest) == FileSystem::status_ok;
			if (!ok)
			{
				handleErrorPolicy(ep, "can't rename file: %s to %s", src, dest);
			}

			return ok;
		}

		unsigned int read(handle h, void *dest, unsigned int destSize)
		{			
			fileHandle *fh = (fileHandle *)h;
			OX_ASSERT(fh && dest);
			LOGD("read file %x %d", fh, destSize);

			return fh->read(dest, destSize);
		}

		void read(const char *file, buffer &dest, error_policy ep)
		{
			dest.data.resize(0);

			autoClose ac(open(file, "rb", ep));
			if (ac.getHandle())
			{
				read(ac.getHandle(), dest);
			}
		}

		unsigned int read(handle fh_, buffer &dest)
		{
			fileHandle *fh = (fileHandle*)fh_;

			unsigned int size  = fh->getSize();
			if (!size)
				return 0;

			dest.data.resize(size);
			unsigned int t = fh->read(&dest.data[0], size);
			LOGD("read file %x %d", fh, t);
			return t;
		}

		void write(handle fh_, const void *data, unsigned int size)
		{
			fileHandle *fh = (fileHandle*)fh_;
			fh->write(data, size);
		}

		

		bool exists(const char *file)
		{
			return _nfs.isExists(file);
		}

		bool makeDirectory(const char *path)
		{
			return _nfs.makeDirectory(path) == FileSystem::status_ok;
		}

		void deleteDirectory(const char *path)
		{
#if __S3E__
			s3eFileDeleteDirectory(path);
#else
#ifdef WIN32
			_rmdir(path);
#else
			rmdir(path);
#endif
#endif
		}

		void write(const char *file, const buffer &data, error_policy ep)
		{
			autoClose ac(open(file, "w", ep));
			if (!ac.getHandle())
				return;
			write(ac.getHandle(), data.getData(), data.getSize());
		}

		file::STDFileSystem &fs()
		{
			return _nfs;
		}

		file::STDFileSystem &wfs()
		{
			return _nfsWrite;
		}
	}
}