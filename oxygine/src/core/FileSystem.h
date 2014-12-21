#pragma once
#include "oxygine_include.h"
#include "Object.h"

namespace oxygine
{
	namespace file
	{
		class fileHandle: public ObjectBase
		{
		public:
			virtual void release() = 0;
			virtual unsigned int read(void *dest, unsigned int size) = 0;
			virtual unsigned int write(const void *src, unsigned int size) = 0;
			virtual unsigned int getSize() const = 0;
			virtual int			 seek(unsigned int offset, int whence) = 0;
			virtual unsigned int tell() const = 0;
		};


		class FileSystem
		{
		public:
			enum status
			{
				status_error,
				status_ok,
			};

			FileSystem(bool readonly);
			~FileSystem();

			void setPrefix(const char *str);

			bool isExists(const char *file);
			bool isExistsHere(const char *file);

			status deleteFile(const char* file);
			status renameFile(const char* src, const char *dest);
			status makeDirectory(const char *path);
			status deleteDirectory(const char *path);

			status open(const char *file, const char *mode, error_policy ep, fileHandle *&fh);

			void mount(FileSystem *fs);
			void unmount(FileSystem *fs);

		protected:
			virtual status _open(const char *file, const char *mode, error_policy ep, fileHandle*&) = 0;
			virtual bool _isExists(const char *file);
			virtual status _deleteFile(const char* file) = 0;
			virtual status _makeDirectory(const char* path) = 0;
			virtual status _deleteDirectory(const char* path) = 0;
			virtual status _renameFile(const char* src, const char *dest) = 0;

			char _prefix[32];
			bool _readonly;

			typedef std::vector<FileSystem*> filesystems;
			filesystems _filesystems;
		};

		void mount(FileSystem *fs);
		void unmount(FileSystem *fs);
	}
}