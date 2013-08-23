#pragma once
#include <vector>
#include "oxygine.h"

namespace oxygine
{
	/**Input/Output files api.*/

	/**
	some notes:
	setExtendedFolder could be used as hook for High Definition art assets.
		If file is missing in additional folder it would be searched from root resources folder.
		here is folders structure example:
	root:
		images/ \n
		atlasses\ \n
		xmls/ \n
		fonts/ \n
		HD/images/ \n
		  /fonts/ \n
		if you want use HD resources you should add setAdditionalFolder("HD");
	*/

	namespace file
	{
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

		void deleteFile(const char *path);
		void rename(const char *src, const char *dest);
		void makeDirectory(const char *path);
		void deleteDirectory(const char *path);

		/**setExtendedFolder sets additional "hook" folder where files would be searched initially.*/
		void setExtendedFolder(const char *folder);

		class autoClose
		{
		public:
			autoClose(handle h):_handle(h){}
			~autoClose(){if (_handle) close(_handle);}

			handle getHandle() const {return _handle;}

		private:
			handle _handle;
		};
	}
}