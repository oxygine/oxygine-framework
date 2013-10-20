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
#endif
#endif



//#define LOGD(...) oxygine::log::messageln(__VA_ARGS__)
#define LOGD(...) 


//#define LOGD(...) {}

namespace oxygine
{
	namespace file
	{
		string _additionalFolder;
		STDFileSystem _nfs;

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
			autoClose ac(open(file, "w", ep));
			if (!ac.getHandle())
				return;
			write(ac.getHandle(), data.getData(), data.getSize());
		}

		void setExtendedFolder(const char *folder)
		{
			_nfs.setExtendedFolder(folder);			
		}

		file::FileSystem &fs()
		{
			return _nfs;
		}
	}
}