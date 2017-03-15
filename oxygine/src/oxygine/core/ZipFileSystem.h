#pragma once
#include "../oxygine-include.h"
#include <vector>
#include "FileSystem.h"
#include "../minizip/unzip.h"
#include "../core/file.h"
#include "../core/Mutex.h"

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

            void add(const char* name);
            void add(const unsigned char* data, unsigned int size);
            void add(std::vector<char>& data);

            void update();

            bool read(const char* name, file::buffer& bf);
            bool read(const file_entry* entry, file::buffer& bf);
            bool isExists(const char* name);

            const file_entry*   getEntryByName(const char* name);
            const file_entry*   getEntry(int index);
            size_t              getNumEntries() const;

            const char* getZipFileName(int i) const { return _zps[i].name; }

        private:
            void read(unzFile zp);

            bool _sort;

            typedef std::vector<file_entry> files;
            files _files;

            struct zpitem
            {
                unzFile handle;
                std::vector<char> data;
                char name[255];

                zpitem() : handle(0) { name[0] = 0; }
            };
            typedef std::vector<zpitem> zips;
            zips _zps;

            Mutex _lock;
        };

        bool read(file_entry* entry, file::buffer& bf);


        class ZipFileSystem: public FileSystem
        {
        public:
            ZipFileSystem(): FileSystem(true) {}

            /**add zip from file*/
            void add(const char* zip);
            //add zip from memory, data should not be deleted
            void add(const unsigned char* data, unsigned int size);
            //add zip from memory, vector would be swapped (emptied)
            void add(std::vector<char>& data);
            void reset();

        protected:

            Zips _zips;

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