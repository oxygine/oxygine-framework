#include "FileSystem.h"
#include <string.h>

namespace oxygine
{
	namespace file
	{
		FileSystem::FileSystem(bool readonly):_readonly(readonly)
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

		bool starts_with(const char *string, const char *prefix)
		{
			while(*prefix)
			{
				if(*prefix++ != *string++)
					return false;
			}

			return true;
		}

		bool FileSystem::isExists(const char *file)
		{
			for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)
			{
				FileSystem *fs = *i;
				if (fs->_isExists(file))
					return true;
			}

			if (!starts_with(file, _prefix))
				return status_error;

			return _isExists(file);
		}

		FileSystem::status FileSystem::deleteFile(const char* file)
		{
			for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)
			{
				FileSystem *fs = *i;
				if (fs->deleteFile(file) == status_ok)
					return status_ok;
			}
			
			if (_readonly)
				return status_error;

			if (!starts_with(file, _prefix))
				return status_error;

			return _deleteFile(file);
		}

		FileSystem::status FileSystem::renameFile(const char* src, const char *dest)
		{
			for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)
			{
				FileSystem *fs = *i;
				if (fs->renameFile(src, dest) == status_ok)
					return status_ok;
			}

			if (_readonly)
				return status_error;

			if (!starts_with(src, _prefix))
				return status_error;

			return _renameFile(src, dest);
		}

		bool FileSystem::isExistsHere(const char *file)
		{
			return _isExists(file);
		}

	

		FileSystem::status FileSystem::open(const char *file, const char *mode, error_policy ep, fileHandle *&fh)
		{
			//file += strlen(_prefix);
			for (filesystems::reverse_iterator i = _filesystems.rbegin(); i != _filesystems.rend(); ++i)
			{
				FileSystem *fs = *i;
				status st = fs->open(file, mode, ep, fh);
				if (st == status_ok)
					return st;
			}

			if (_readonly)
			{
				bool write = mode[0] == 'W' || mode[0] == 'w';
				if (write)
					return status_error;
			}

			if (!starts_with(file, _prefix))
				return status_error;			

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