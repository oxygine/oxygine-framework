#include <string.h>
#include <map>
#include "../minizip/ioapi_mem.h"
#include "core/ZipFileSystem.h"
#include "core/Object.h"
#include "utils/stringUtils.h"
#include <algorithm>
//#include "utils/utils.h"

#ifdef __S3E__
#include "s3eFile.h"
#endif

namespace oxygine
{
    namespace file
    {
        bool sortFiles(const file_entry& ob1, const file_entry& ob2)
        {
            return strcmp_cns(ob1.name, ob2.name) < 0;
        }

        bool readEntry(const file_entry* entry, file::buffer& bf)
        {
            bf.data.clear();
            int r = unzGoToFilePos(entry->zp, const_cast<unz_file_pos*>(&entry->pos));
            OX_ASSERT(r == UNZ_OK);

            unz_file_info file_info;
            r = unzGetCurrentFileInfo(entry->zp, &file_info, 0, 0, 0, 0, 0, 0);
            OX_ASSERT(r == UNZ_OK);

            unzOpenCurrentFile(entry->zp);

            bf.data.resize(file_info.uncompressed_size);
            r = unzReadCurrentFile(entry->zp, &bf.data.front(), (int)bf.data.size());
            OX_ASSERT(r == (int)file_info.uncompressed_size);

            unzCloseCurrentFile(entry->zp);
            return true;
        }

        Zips::Zips(): _sort(false), _lock(true)
        {

        }

        Zips::~Zips()
        {
            reset();
        }

        void Zips::read(unzFile zp)
        {
            //MutexAutoLock al(_lock);

            do
            {
                unz_file_pos pos;
                unzGetFilePos(zp, &pos);

                file_entry entry;
                unzGetCurrentFileInfo(zp, 0, entry.name, sizeof(entry.name) - 1, 0, 0, 0, 0);
                entry.pos = pos;
                entry.zp = zp;

                _files.push_back(entry);

            }
            while (unzGoToNextFile(zp) != UNZ_END_OF_LIST_OF_FILE);

            _sort = true;
        }

        void Zips::add(const unsigned char* data, unsigned int size)
        {
            MutexAutoLock al(_lock);

            zlib_filefunc_def ff;
            fill_memory_filefunc(&ff);

            zmemdata dta;
            dta.data = (char*)data;
            dta.size = size;

            unzFile zp = unzOpen2((const char*)&dta, &ff);
            OX_ASSERT(zp);
            if (!zp)
                return;


            zpitem item;
            item.handle = zp;
            _zps.push_back(item);

            read(zp);
        }

        void Zips::add(std::vector<char>& data)
        {
            MutexAutoLock al(_lock);

            zlib_filefunc_def ff;
            fill_memory_filefunc(&ff);

            zmemdata dta;
            dta.data = (char*)&data.front();
            dta.size = data.size();

            unzFile zp = unzOpen2((const char*)&dta, &ff);
            OX_ASSERT(zp);
            if (!zp)
                return;

            _zps.push_back(zpitem());
            zpitem& item = _zps.back();
            item.handle = zp;
            std::swap(item.data, data);

            read(zp);
        }

        voidpf ZCALLBACK ox_fopen(voidpf opaque, const char* filename, int mode)
        {
            return file::open(filename, "rb");
        }

        uLong ZCALLBACK ox_fread(voidpf opaque, voidpf stream, void* buf, uLong size)
        {
            return file::read((handle)stream, buf, (unsigned int)size);
        }

        /*
        uLong ZCALLBACK ox_fwriteOF(voidpf opaque, voidpf stream, const void* buf, uLong size)
        {

        }
        */

        long ZCALLBACK ox_ftell(voidpf opaque, voidpf stream)
        {
            return file::tell((handle)stream);
        }

        long ZCALLBACK ox_fseek(voidpf opaque, voidpf stream, uLong offset, int origin)
        {
            file::seek((handle)stream, (unsigned int)offset, origin);
            return 0;
        }

        int ZCALLBACK ox_fclose(voidpf opaque, voidpf stream)
        {
            file::close((handle)stream);
            return 0;
        }

        int ZCALLBACK ox_ferror(voidpf opaque, voidpf stream)
        {
            return 0;
        }

        void Zips::remove(const char* name)
        {
            MutexAutoLock al(_lock);

            for (size_t i = 0; i < _zps.size(); ++i)
            {
                zpitem& item = _zps[i];
                if (!strcmp(item.name, name))
                {
                    for (size_t n = 0; n < _files.size();)
                    {
                        file_entry& entry = _files[n];
                        if (entry.zp == item.handle)
                        {
                            _files.erase(_files.begin() + n);
                        }
                        else
                            ++n;
                    }

                    unzClose(item.handle);

                    _zps.erase(_zps.begin() + i);
                    break;
                }
            }
        }

        void Zips::add(const char* name)
        {
            MutexAutoLock al(_lock);

            zlib_filefunc_def zpfs;
            memset(&zpfs, 0, sizeof(zpfs));

            zpfs.zopen_file = ox_fopen;
            zpfs.zread_file = ox_fread;
            zpfs.zwrite_file = 0;
            zpfs.ztell_file = ox_ftell;
            zpfs.zseek_file = ox_fseek;
            zpfs.zclose_file = ox_fclose;
            zpfs.zerror_file = ox_ferror;
            zpfs.opaque = (void*)_zps.size();
            //zpfs.opaque = name;

            unzFile zp = unzOpen2(name, &zpfs);//todo, optimize search in zip
            OX_ASSERT(zp);
            if (!zp)
                return;

            zpitem item;
            item.handle = zp;
            strcpy(item.name, name);
            _zps.push_back(item);

            read(zp);
        }

        void Zips::update()
        {
            std::sort(_files.begin(), _files.end(), sortFiles);
#ifdef OX_DEBUG
            for (int i = 0; i < (int)_files.size() - 1; ++i)
            {
                OX_ASSERT(strcmp(_files[i].name, _files[i + 1].name) != 0);
            }
#endif
            log::messageln("ZipFS, total files: %d", _files.size());
        }

        bool Zips::isExists(const char* name)
        {
            MutexAutoLock al(_lock);
            return getEntryByName(name) != 0;
        }

        bool Zips::read(const char* name, file::buffer& bf)
        {
            MutexAutoLock al(_lock);

            const file_entry* entry = getEntryByName(name);
            if (!entry)
                return false;
            return readEntry(entry, bf);
        }

        bool Zips::read(const file_entry* entry, file::buffer& bf)
        {
            MutexAutoLock al(_lock);
            return readEntry(entry, bf);
        }

        void Zips::reset()
        {
            MutexAutoLock al(_lock);
            for (zips::iterator i = _zps.begin(); i != _zps.end(); ++i)
            {
                zpitem& f = *i;
                unzClose(f.handle);
            }

            _zps.clear();
            _files.clear();
        }


        bool findEntry(const file_entry& g, const char* name)
        {
            return strcmp_cns(g.name, name) < 0;
        }

        const file_entry* Zips::getEntryByName(const char* name)
        {
            if (_sort)
            {
                update();
                _sort = false;
            }

            files::const_iterator it = std::lower_bound(_files.begin(), _files.end(), name, findEntry);
            if (it != _files.end())
            {
                const file_entry& g = *it;
                if (!strcmp_cns(g.name, name))
                    return &g;
            }

            return 0;
        }

        const file_entry* Zips::getEntry(int index)
        {
            if (_sort)
            {
                update();
                _sort = false;
            }

            return &_files[index];
        }

        size_t Zips::getNumEntries() const
        {
            return _files.size();
        }


        class fileHandleZip: public fileHandle
        {
        public:

            fileHandleZip(FileSystem* fs, const file_entry* entry): fileHandle(fs), _entry(entry)
            {
                int r = 0;
                r = unzGoToFilePos(entry->zp, const_cast<unz_file_pos*>(&entry->pos));
                OX_ASSERT(r == UNZ_OK);
                r = unzOpenCurrentFile(entry->zp);
                OX_ASSERT(r == UNZ_OK);
            }

            void release()
            {
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

            const file_entry* _entry;
        };

        class fileHandleZipStreaming : public fileHandle
        {
        public:
            file::handle _h;
            z_off_t _pos;
            z_off_t _size;
            long _cpos;

            fileHandleZipStreaming(FileSystem* fs, const file_entry* entry, const Zips& z): fileHandle(fs), _cpos(0)
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
            }

            ~fileHandleZipStreaming()
            {
                int q = 0;
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
            log::messageln("ZipFileSystem::add %s", zip);
            _zips.add(zip);
        }

        void ZipFileSystem::add(const unsigned char* data, unsigned int size)
        {
            _zips.add(data, size);
        }

        void ZipFileSystem::add(std::vector<char>& data)
        {
            _zips.add(data);
        }


        void ZipFileSystem::remove(const char* zip)
        {
            _zips.remove(zip);
        }

        void ZipFileSystem::reset()
        {
            _zips.reset();
        }

        FileSystem::status ZipFileSystem::_read(const char* file, file::buffer& bf, error_policy ep)
        {
            if (_zips.read(file, bf))
                return status_ok;

            return status_error;
        }

        FileSystem::status ZipFileSystem::_open(const char* file, const char* mode, error_policy ep, file::fileHandle*& fh)
        {
            MutexAutoLock lock(_zips._lock);

            const file_entry* entry = _zips.getEntryByName(file);
            if (entry)
            {
                if (*mode == 's')
                    fh = new fileHandleZipStreaming(this, entry, _zips);
                else
                    fh = new fileHandleZip(this, entry);
                return status_ok;
            }

            handleErrorPolicy(ep, "can't find zip file entry: %s", file);
            return status_error;
        }

        bool ZipFileSystem::_isExists(const char* file)
        {
            return _zips.isExists(file);
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