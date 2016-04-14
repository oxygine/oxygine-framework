#pragma once
#include "oxygine_include.h"
#include <vector>
#include "Object.h"
#include "FileSystem.h"

namespace oxygine
{
    namespace file
    {
        class STDFileSystem;

        struct _handle_ {};
        typedef _handle_* handle;

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
        handle open(const char* file, const char* mode, error_policy ep = ep_show_error);

        /**Closes opened file handle*/
        void close(handle);

        int seek(handle, unsigned int offset, int whence);
        unsigned int tell(handle);

        /**Reads bytes into user memory*/
        unsigned int read(handle, void* dest, unsigned int size);

        /**Reads bytes into destination buffer with stdio flags = "rb". Clears existing buffer*/
        void read(const char* file, buffer& dest, error_policy ep = ep_show_error);

        /**Reads bytes into destination buffer with stdio flags = "wb"*/
        unsigned int read(handle, buffer& dest);

        /**Writes bytes to file*/
        void write(handle, const void* data, unsigned int size);

        /**Writes bytes to file*/
        void write(const char* file, const buffer& data, error_policy ep = ep_show_error);
        void write(const char* file, const void* data, unsigned int size, error_policy ep = ep_show_error);

        /**Is file exists?*/
        bool exists(const char* file);

        /**returns opened file size*/
        unsigned int size(handle);

        /**Deletes file*/
        bool deleteFile(const char* path, error_policy ep = ep_show_warning);

        /**Renames file*/
        bool rename(const char* src, const char* dest, error_policy ep = ep_show_warning);

        /**Makes directory. Not recursive*/
        bool makeDirectory(const char* path);

        /**Deletes empty directory*/
        void deleteDirectory(const char* path);

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
    }
}