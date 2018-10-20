#include "ZipFileSystem.h"
#include "../core/Object.h"
#include "../minizip/ioapi_mem.h"
#include "../utils/stringUtils.h"
#include <algorithm>
#include <map>
#include <string.h>
#include "Zips.h"

namespace oxygine
{
    namespace file
    {
       
        ZipFileSystem::ZipFileSystem() : FileSystem(true) 
        {
            _zips = new Zips();
        }



        class fileHandleZip: public fileHandle
        {
        public:

            fileHandleZip(FileSystem* fs, file_entry* entry): fileHandle(fs), _entry(entry)
            {
                int r = unzGoToFilePos(entry->zp, const_cast<unz_file_pos*>(&entry->pos));
                OX_ASSERT(r == UNZ_OK);
                r = unzOpenCurrentFile(entry->zp);
                OX_ASSERT(r == UNZ_OK);
                _entry->refs++;
            }

            void release()
            {
                ZipFileSystem* zfs = static_cast<ZipFileSystem*>(_fs);
                MutexAutoLock lock(zfs->_zips->getMutex());
                _entry->refs--;

                int r = unzCloseCurrentFile(_entry->zp);
                OX_ASSERT(r == UNZ_OK);
                delete this;
            }

            int seek(unsigned int offset, int whence)
            {
                OX_ASSERT(!"not implemented");
                return 0;
            }

            unsigned int tell() const
            {
                OX_ASSERT(!"not implemented");
                return 0;
            }

            unsigned int read(void* dest, unsigned int size)
            {
                return unzReadCurrentFile(_entry->zp, dest, size);
            }

            unsigned int write(const void* src, unsigned int size)
            {
                return 0;
            }

            unsigned int getSize() const
            {
                unz_file_info file_info;
                unzGetCurrentFileInfo(_entry->zp, &file_info, 0, 0, 0, 0, 0, 0);
                return (unsigned int) file_info.uncompressed_size;
            }

            file_entry* _entry;
        };

        class fileHandleZipStreaming : public fileHandle
        {
        public:
            file::handle _h;
            z_off_t _pos;
            z_off_t _size;
            long _cpos;
            file_entry* _entry;

            fileHandleZipStreaming(FileSystem* fs, file_entry* entry, const Zips& z): fileHandle(fs), _cpos(0), _entry(entry)
            {
                int r = 0;
                r = unzGoToFilePos(entry->zp, const_cast<unz_file_pos*>(&entry->pos));
                OX_ASSERT(r == UNZ_OK);

                r = unzOpenCurrentFile(entry->zp);
                OX_ASSERT(r == UNZ_OK);


                void* ptr;
                r = unzRealTell(entry->zp, &_pos, &_size, &ptr);
                OX_ASSERT(r == UNZ_OK);

                zlib_filefunc_def* f = (zlib_filefunc_def*)ptr;
                const char* ssss = z.getZipFileName((int)(size_t)f->opaque);

                _h = file::open(ssss, "rb");
                file::seek(_h, static_cast<unsigned int>(_pos), SEEK_SET);

                unzCloseCurrentFile(entry->zp);
                entry->refs++;
            }

            ~fileHandleZipStreaming()
            {
                ZipFileSystem* zfs = static_cast<ZipFileSystem*>(_fs);
                MutexAutoLock lock(zfs->_zips->getMutex());
                _entry->refs--;
            }

            void release()
            {
                file::close(_h);
                delete this;
            }

            unsigned int read(void* dest, unsigned int size)
            {
                if (_cpos + (long)size > _size)
                    size = (unsigned int)(_size - _cpos);

                unsigned int ret = file::read(_h, dest, size);
                _cpos += ret;

                return ret;
            }

            unsigned int write(const void* src, unsigned int size)
            {
                OX_ASSERT(0);
                return 0;
            }

            unsigned int getSize() const
            {
                return (unsigned int)_size;
            }

            int          seek(unsigned int offset, int whence)
            {
                switch (whence)
                {
                    case SEEK_SET:
                        _cpos = offset;
                        return file::seek(_h, (unsigned int)(_pos + offset), SEEK_SET);
                    case SEEK_CUR:
                        _cpos += offset;
                        return file::seek(_h, offset, SEEK_CUR);
                    case SEEK_END:
                        _cpos = _size;
                        return file::seek(_h, offset, SEEK_END);
                    default:
                        break;
                }
                OX_ASSERT(0);
                return 0;
            }

            unsigned int tell() const
            {
                return (unsigned int)_cpos;
            }

        };


        void ZipFileSystem::add(const char* zip)
        {
            logs::messageln("ZipFileSystem::add %s", zip);
            _zips->add(zip);
        }

        void ZipFileSystem::add(const unsigned char* data, unsigned int size)
        {
            _zips->add(data, size);
        }

        void ZipFileSystem::add(std::vector<char>& data)
        {
            _zips->add(data);
        }


        void ZipFileSystem::remove(const char* zip)
        {
            _zips->remove(zip);
        }

        void ZipFileSystem::reset()
        {
            _zips->reset();
        }

        FileSystem::status ZipFileSystem::_read(const char* file, file::buffer& bf, error_policy ep)
        {
            if (_zips->read(file, bf))
                return status_ok;

            return status_error;
        }

        FileSystem::status ZipFileSystem::_open(const char* file, const char* mode, error_policy ep, file::fileHandle*& fh)
        {
            MutexAutoLock lock(_zips->_lock);

            file_entry* entry = _zips->getEntryByName(file);
            if (entry)
            {
                if (*mode == 's')
                    fh = new fileHandleZipStreaming(this, entry, *_zips);
                else
                    fh = new fileHandleZip(this, entry);
                return status_ok;
            }

            handleErrorPolicy(ep, "can't find zip file entry: %s", file);
            return status_error;
        }

        bool ZipFileSystem::_isExists(const char* file)
        {
            return _zips->isExists(file);
        }

        FileSystem::status ZipFileSystem::_deleteFile(const char* file)
        {
            return status_error;
        }

        FileSystem::status ZipFileSystem::_makeDirectory(const char* file)
        {
            return status_error;
        }

        FileSystem::status ZipFileSystem::_deleteDirectory(const char* file)
        {
            return status_error;
        }

        FileSystem::status ZipFileSystem::_renameFile(const char* src, const char* dest)
        {
            return status_error;
        }
    }
}