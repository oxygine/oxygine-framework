#pragma once
#include "oxygine_include.h"
#include <vector>
#include "Object.h"
#include "oxygine.h"
#include "FileSystem.h"

namespace oxygine
{
	namespace file
	{
		class STDFileSystem;

		class buffer
		{
		public:		
			const void *getData() const {if (data.empty()) return 0; return &data[0];}
			unsigned int getSize() const {return (unsigned int)data.size();}

			typedef std::vector<unsigned char> buff;
			buff data;
		};

		typedef void* handle;

		/**Opens file for reading (mode = "r") or writing (mode = "w"). If file is missing returns zero.*/
		handle open(const char *file, const char *mode, error_policy ep = ep_show_error);

		/**Closes opened file handle*/
		void close(handle);

		int seek(handle, unsigned int offset, int whence);
		unsigned int tell(handle);

		/**Reads bytes into user memory*/
		unsigned int read(handle, void *dest, unsigned int size);

		/**Reads bytes into destination buffer. Clears existing buffer*/
		void read(const char *file, buffer &dest, error_policy ep = ep_show_error);

		/**Reads bytes into destination buffer*/
		unsigned int read(handle, buffer &dest);

		/**Writes bytes to file*/
		void write(handle, const void *data, unsigned int size);

		/**Writes bytes to file*/
		void write(const char *file, const buffer &data, error_policy ep = ep_show_error);

		/**Is file exists?*/
		bool exists(const char *file);

		bool deleteFile(const char *path, error_policy ep = ep_show_warning);
		bool rename(const char *src, const char *dest, error_policy ep = ep_show_warning);
		bool makeDirectory(const char *path);
		void deleteDirectory(const char *path);
		
		//returns main fs
		file::STDFileSystem &fs();
		//returns writable fs
		file::STDFileSystem &wfs();

		class autoClose
		{
		public:
			autoClose(handle h):_handle(h){}
			~autoClose(){if (_handle) close(_handle);}

			handle getHandle() const {return _handle;}

		private:
			//non copyable
			autoClose(const autoClose&);			
			const autoClose& operator=(const autoClose&);	

			handle _handle;
		};
	}
}