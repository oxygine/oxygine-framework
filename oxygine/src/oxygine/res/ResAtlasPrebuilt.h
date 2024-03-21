#pragma once
#include "ResAtlas.h"
#ifdef FORCE_HPC
#define HPC_DEFAULT true
#else
#define HPC_DEFAULT false
#endif

namespace oxygine
{

    class ResAtlasPrebuilt : public ResAtlas
    {
    public:

        ResAtlasPrebuilt(CreateResourceContext& context);

    protected:
    };
}