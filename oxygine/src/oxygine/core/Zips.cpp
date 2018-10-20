#include "Zips.h"
#include "../core/Object.h"
#include "../minizip/ioapi_mem.h"
#include "../utils/stringUtils.h"
#include <algorithm>
#include <map>
#include <string.h>

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

    Zips::Zips() : _lock(true)
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
            entry.refs = 0;
            entry.pos = pos;
            entry.zp = zp;

            char* str = entry.name;
            for (int i = 0; str[i]; i++)
                str[i] = tolower(str[i]);


            OX_ASSERT(_files.find(entry.name) == _files.end());

            _files[entry.name] = entry;

        } while (unzGoToNextFile(zp) != UNZ_END_OF_LIST_OF_FILE);

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
                for (files::iterator it = _files.begin(); it != _files.end();)
                {
                    file_entry& entry = it->second;
                    if (entry.zp == item.handle)
                    {
                        OX_ASSERT(entry.refs == 0);
                        it = _files.erase(it);
                    }
                    else
                        ++it;
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


#ifdef OX_DEBUG
        for (files::iterator i = _files.begin(); i != _files.end(); ++i)
        {
            OX_ASSERT(i->second.refs == 0);
        }

#endif
        _zps.clear();
        _files.clear();
    }

    file_entry* Zips::getEntryByName(const char* name)
    {
        char str[255];
        char* p = str;
        while (*name)
        {
            *p = tolower(*name);
            name++;
            ++p;
        }
        *p = 0;

        files::iterator it = _files.find(str);
        if (it != _files.end())
            return &it->second;

        return 0;
    }

    /*
    const file_entry* Zips::getEntry(int index)
    {
    return &_files[index];
    }

    size_t Zips::getNumEntries() const
    {
    return _files.size();
    }
    */
     }
}