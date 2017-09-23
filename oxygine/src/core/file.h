#pragma once
#include "oxygine-include.h"
#include <vector>
#include "Object.h"
#include "FileSystem.h"
#include <string>

namespace oxygine
{
    namespace file
    {
        class STDFileSystem;


        typedef file::fileHandle* handle;

        /**memory buffer for files IO operations, emulates std::vector */
        class buffer
        {
        public:
            typedef unsigned char uchar;

            uchar& front() { return data.front(); }
            const uchar& front() const { return data.front(); }

            uchar& back() { return data.back(); }
            const uchar& back() const { return data.back(); }

            uchar& operator[](size_t i) { return data[i]; }
            const uchar& operator[](size_t i) const { return data[i]; }

            uchar& at(size_t i) { return data.at(i); }
            const uchar& at(size_t i) const { return data.at(i); }

            void push_back(uchar v) { return data.push_back(v); }
            uchar pop_back() { uchar v = data.back();  data.pop_back(); return v; }
            void reserve(size_t v) { data.reserve(v); }
            void resize(size_t v) { data.resize(v); }

            size_t size() const { return data.size(); }
            bool empty() const { return data.empty(); }

            const void* getData() const {if (data.empty()) return 0; return &data[0];}
            void* getData() { if (data.empty()) return 0; return &data[0]; }
            unsigned int getSize() const {return (unsigned int)data.size();}

            std::string getString() const {return std::string((char*)&data[0], size());}

            typedef std::vector<uchar> buff;
            buff data;
        };

        /**Opens file for binary reading (mode = "rb") or binary writing (mode = "wb"). If file is missing returns zero.*/
        handle open(const std::string& file, const char* mode, error_policy ep = ep_show_error);

        /**Closes opened file handle*/
        void close(handle);

        /**Seeks to file position, whence could be SEEK_SET, SEEK_END, SEEK_CUR*/
        int seek(handle, unsigned int offset, int whence);

        /**Returns current file position*/
        unsigned int tell(handle);

        /**Reads bytes into user memory*/
        unsigned int read(handle, void* dest, unsigned int size);

        /**Reads bytes into destination buffer with stdio flags = "rb". Clears existing buffer*/
        bool read(const std::string& file, buffer& dest, error_policy ep = ep_show_error);

        /**Reads bytes into destination buffer with stdio flags = "wb"*/
        unsigned int read(handle, buffer& dest);

        /**Writes bytes to file*/
        unsigned int write(handle, const void* data, unsigned int size);

        /**Writes bytes to file*/
        unsigned int write(const std::string& file, const buffer& data, error_policy ep = ep_show_error);
        unsigned int write(const std::string& file, const void* data, unsigned int size, error_policy ep = ep_show_error);

        /**Is file exists?*/
        bool exists(const std::string& file);

        /**returns opened file size*/
        unsigned int size(handle);

        /**Deletes file*/
        bool deleteFile(const std::string& path, error_policy ep = ep_show_warning);

        /**Renames file*/
        bool rename(const std::string& src, const std::string& dest, error_policy ep = ep_show_warning);

        /**Makes directory. Not recursive*/
        bool makeDirectory(const std::string& path);
        void makeDirectories(const std::string& path);

        /**Deletes empty directory*/
        void deleteDirectory(const std::string& path);

        /**Returns primary read only FileSystem*/
        file::STDFileSystem& fs();

        /**Returns writable FileSystem*/
        file::STDFileSystem& wfs();

        /**Mounts additional FileSystem*/
        void mount(FileSystem* fs);

        /**Unmounts additional FileSystem*/
        void unmount(FileSystem* fs);

        class autoClose
        {
        public:
            autoClose(handle h): _handle(h) {}
            ~autoClose() {if (_handle) close(_handle);}

            handle getHandle() const {return _handle;}

        private:
            //non copyable
            autoClose(const autoClose&);
            const autoClose& operator=(const autoClose&);

            handle _handle;
        };



        /**you don't need to call "file::init" yourself. It would be called from core::init. Use it only when you need access to filesystem before core::init*/
        void init(const char* company, const char* app);

        /**for internal purposes*/
        void free();
    }
}