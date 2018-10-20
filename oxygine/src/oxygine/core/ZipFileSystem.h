#pragma once
#include "../oxygine-include.h"
#include "FileSystem.h"

namespace oxygine
{
    namespace file
    {
        class ZipFileSystem: public FileSystem
        {
        public:
            ZipFileSystem();

            /**add zip from file*/
            void add(const char* zip);
            void remove(const char* zip);
            //add zip from memory, data should not be deleted
            void add(const unsigned char* data, unsigned int size);
            //add zip from memory, vector would be swapped (emptied)
            void add(std::vector<char>& data);
            void reset();

        protected:

            friend class fileHandleZip;
            friend class fileHandleZipStreaming;

            class Zips* _zips;

            status _read(const char* file, file::buffer&, error_policy ep);
            status _open(const char* file, const char* mode, error_policy ep, file::fileHandle*& fh);
            status _deleteFile(const char* file);
            status _makeDirectory(const char* file);
            status _deleteDirectory(const char* file);
            status _renameFile(const char* src, const char* dest);
            bool _isExists(const char* file);

        };
    }
}