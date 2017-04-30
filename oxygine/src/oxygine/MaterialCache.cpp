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

        MaterialX* free = 0;
        for (auto m_ : _materials)
        {
            MaterialX* mat = m_.get();
            if (mat->_compare != cm)
                continue;
            if (mat->_ref_counter == 1)
                free = mat;
            if (mat->_hash != hash)
                continue;
            bool same = cm(mat, &other);
            if (same)
                return mat;
        }
        if (free)
        {
            free->copyFrom(other);
            free->_hash = hash;
            free->_compare = cm;
            return free;
        }

        MaterialX* copy = other.clone();
        copy->_hash = hash;
        copy->_compare = cm;
        _materials.push_back(copy);

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