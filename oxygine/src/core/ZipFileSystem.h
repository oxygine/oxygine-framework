#pragma once
#include <vector>
#include "FileSystem.h"
#include "minizip/unzip.h"
#include "core/file.h"
using namespace std;
using namespace oxygine;

namespace oxygine
{
namespace file
{
	struct file_entry
	{
		unzFile zp;
		char name[128];
		unz_file_pos pos;
	};

	class Zips
	{
	public:
		Zips();
		~Zips();

		void reset();

		void add(const char *name);
		void add(const unsigned char* data, unsigned int size);
		
		void update();

		bool read(const char *name, file::buffer &bf);

		const file_entry *getEntry(const char *name);

	private:
		void add(unzFile zp);

		bool _sort;

		typedef vector<file_entry> files;
		files _files;

		typedef vector<unzFile> zips;
		zips _zps;
	};

	bool read(file_entry *entry, file::buffer &bf);


	class ZipFileSystem: public FileSystem
	{
	public:
		ZipFileSystem(){}

		void add(const char *zip);
		void add(const unsigned char* data, unsigned int size);
		void reset();

	protected:

		Zips _zips;

		status _open(const char *file, const char *mode, error_policy ep, file::fileHandle *&fh);
	};
}
}