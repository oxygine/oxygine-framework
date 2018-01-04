#include "MaterialCache.h"
#include "Material.h"
#include "core/oxygine.h"

namespace oxygine
{
    Material* MaterialCache::clone_(const Material& other)
    {
        MutexAutoLock alock(_lock);

        size_t hash;
        Material::compare cm;
        other.update(hash, cm);


        materials::iterator itl = _materials.find(hash);

        if (itl != _materials.end())
        {
            Material* sec = itl->second.get();
            if (cm == sec->_compare && cm(sec, &other))
                return sec;

            //hash collision?

            std::pair<materials::iterator, materials::iterator> it = _materials.equal_range(hash);

            itl = it.first;
            itl++;//skip first, already checked

            for (; itl != it.second; itl++)
            {
                Material* sec = itl->second.get();
                if (cm == sec->_compare && cm(sec, &other))
                    return sec;
            }
        }

        _addCounter++;
        if (_addCounter > 30)
            removeUnusedNoLock();

        Material* copy = other.clone();
        copy->_hash = hash;
        copy->_compare = cm;
        _materials.insert(std::make_pair(hash, copy));
        
        return copy;
    }

    void MaterialCache::removeUnusedNoLock()
    {
        _addCounter = 0;
        materials fresh;
        for (auto it = _materials.begin(); it != _materials.end(); it++)
        {
            if (it->second->_ref_counter > 1)
            {
                fresh.insert(std::make_pair(it->second->_hash, it->second));
            }
        }

        std::swap(fresh, _materials);
    }

    void MaterialCache::removeUnused()
    {
        MutexAutoLock alock(_lock);
        removeUnusedNoLock();
    }

    MaterialCache::MaterialCache():_addCounter(0)
    {

    }

    void MaterialCache::clear()
    {
        MutexAutoLock alock(_lock);
        _addCounter = 0;
        _materials.clear();
    }

    static MaterialCache mcache;

    MaterialCache& mc()
    {
        return mcache;
    }
}