#include "FileSystem.h"
#include <string.h>

namespace oxygine
{
	namespace file
	{
		FileSystem::FileSystem()
		{
			_prefix[0] = 0;
		}

		FileSystem::~FileSystem()
		{

		}

		void FileSystem::setPrefix(const char *str)
		{
			strcpy(_prefix, str);
		}

		bool FileSystem::isExists(const char *file)
		{
			for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)
			{
				FileSystem *fs = *i;
				if (fs->_isExists(file))
					return true;
			}

			return _isExists(file);
		}

		bool FileSystem::isExistsHere(const char *file)
		{
			return _isExists(file);
		}

		bool starts_with(const char *string, const char *prefix)
		{
			while(*prefix)
			{
				if(*prefix++ != *string++)
					return false;
			}

			return true;
		}

		FileSystem::status FileSystem::open(const char *file, const char *mode, error_policy ep, fileHandle *&fh)
		{
			if (!starts_with(file, _prefix))
				return status_error;
			//file += strlen(_prefix);
			for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)
			{
				FileSystem *fs = *i;
				status st = fs->open(file, mode, ep, fh);
				if (st == status_ok)
					return st;
			}

			return _open(file, mode, ep, fh);
		}

		void FileSystem::mount(FileSystem *fs)
		{
			filesystems::iterator i = find(_filesystems.begin(), _filesystems.end(), fs);
			OX_ASSERT(i == _filesystems.end());
			_filesystems.push_back(fs);
		}

		void FileSystem::unmount(FileSystem *fs)
		{
			filesystems::iterator i = find(_filesystems.begin(), _filesystems.end(), fs);
			if (i != _filesystems.end())
			{
				_filesystems.erase(i);
			}
		}

		bool FileSystem::_isExists(const char *file)
		{
			fileHandle *fh = 0;
			status s = _open(file, "rb", ep_ignore_error, fh);
			if (fh)
				fh->release();
			return s == status_ok;
		}
	}
}