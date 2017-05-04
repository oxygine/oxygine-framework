#include "MaterialCache.h"
#include "core/oxygine.h"
#include "MaterialX.h"
namespace oxygine
{
    MaterialX* MaterialCache::clone_(const MaterialX& other)
    {
        OX_ASSERT(core::isMainThread());
        size_t hash;
        MaterialX::compare cm;
        other.update(hash, cm);


        materials::iterator itl = _materials.lower_bound(hash);
                
        if (itl != _materials.end())
        {
            MaterialX *sec = itl->second.get();
            if (cm(sec, &other))
                return sec;

            ++itl;

            //same hash but not same object
            materials::iterator ith = _materials.upper_bound(hash);
            for(; itl != ith; itl++)
            { 
                MaterialX *sec = itl->second.get();
                if (cm(sec, &other))
                    return sec;
            }
        }


        MaterialX* copy = other.clone();
        copy->_hash = hash;
        copy->_compare = cm;
        _materials.insert(std::make_pair(hash, copy));

        return copy;
    }

    void MaterialCache::clear()
    {
        _materials.clear();
    }

    static MaterialCache mcache;

    MaterialCache& mc()
    {
        return mcache;
    }
}