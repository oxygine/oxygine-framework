#pragma once
#include "FileSystem.h"
namespace oxygine
{
	namespace file
	{
		class STDFileSystem: public FileSystem
		{
		public:
			STDFileSystem();
			void setExtendedFolder(const char *folder);

		protected:
			string _extended;

			status _open(const char *file_, const char *mode, error_policy ep, fileHandle*& fh);
			bool _isExists(const char *file);
		};
	}	
}