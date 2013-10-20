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
		};


		class FileSystem
		{
		public:
			enum status
			{
				status_error,
				status_ok,
			};

			FileSystem();
			~FileSystem();

			void setPrefix(const char *str);

			bool isExists(const char *file);
			bool isExistsHere(const char *file);

			status open(const char *file, const char *mode, error_policy ep, fileHandle *&fh);

			void mount(FileSystem *fs);
			void unmount(FileSystem *fs);

		protected:
			virtual status _open(const char *file, const char *mode, error_policy ep, fileHandle*&) = 0;
			virtual bool _isExists(const char *file);

			char _prefix[32];

			typedef vector<FileSystem*> filesystems;
			filesystems _filesystems;
		};

		void mount(FileSystem *fs);
		void unmount(FileSystem *fs);
	}
}