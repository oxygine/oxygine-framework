#pragma once
#include "ResAtlas.h"
namespace oxygine
{

    struct atlas_data;

    class ResAtlasGeneric : public ResAtlas
    {
    public:

        int _current;

        //CreateResourceContext _copy;

        //std::string _xmlFolder;

        void nextAtlas(int w, int h, TextureFormat tf, atlas_data& ad, const char* name);
        void applyAtlas(atlas_data& ad, bool linear, bool clamp2edge);

        void loadAtlas(CreateResourceContext& context);
        void loadAtlas2(CreateResourceContext& context);

        void _load(LoadResourcesContext*) override;
        void _unload() override;
    };
}