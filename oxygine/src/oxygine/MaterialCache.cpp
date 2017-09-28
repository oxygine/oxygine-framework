#include "MaterialCache.h"
#include "core/oxygine.h"
#include "MaterialX.h"
namespace oxygine
{
    MaterialX* MaterialCache::clone_(const MaterialX& other)
    {
        MutexAutoLock alock(_lock);

        size_t hash;
        MaterialX::compare cm;
        other.update(hash, cm);


        materials::iterator itl = _materials.find(hash);

        if (itl != _materials.end())
        {
            MaterialX* sec = itl->second.get();
            if (cm == sec->_compare && cm(sec, &other))
                return sec;

            //hash collision?

            std::pair<materials::iterator, materials::iterator> it = _materials.equal_range(hash);

            itl = it.first;
            itl++;//skip first, already checked

            for (; itl != it.second; itl++)
            {
                MaterialX* sec = itl->second.get();
                if (cm == sec->_compare && cm(sec, &other))
                    return sec;
            }
        }


        MaterialX* copy = other.clone();
        copy->_hash = hash;
        copy->_compare = cm;
        _materials.insert(std::make_pair(hash, copy));

        return copy;
    }

    void MaterialCache::removeUnused()
    {
        MutexAutoLock alock(_lock);

        //std::pair<materials::iterator, materials::iterator> it = _materials.begin();

        materials fresh;
        for (auto it = _materials.begin(); it != _materials.end(); it++) {
            if (it->second->_ref_counter > 1)
            {
                fresh.insert(std::make_pair(it->second->_hash, it->second));
            }
        }

        std::swap(fresh, _materials);
    }

    void MaterialCache::clear()
    {
        MutexAutoLock alock(_lock);
        _materials.clear();
    }

    static MaterialCache mcache;

    MaterialCache& mc()
    {
        return mcache;
    }
}