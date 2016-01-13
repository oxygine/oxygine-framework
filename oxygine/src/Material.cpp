#include "Material.h"

namespace oxygine
{
    Material::~Material()
    {
        OX_ASSERT(currentMaterial != this);
        if (currentMaterial == this)
            currentMaterial = 0;
    }


    Material* Material::currentMaterial = 0;
    void Material::setCurrent(Material* next)
    {
        if (currentMaterial == next)
            return;
        if (currentMaterial)
            currentMaterial->finish();
        if (next)
            next->apply(currentMaterial);
        currentMaterial = next;
    }
}