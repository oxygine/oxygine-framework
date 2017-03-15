#pragma once
#include "oxygine-include.h"
#include "Resource.h"
namespace oxygine
{
    class ResStarlingAtlas: public Resource
    {
        INHERITED(Resource);
    public:
        static Resource* create(CreateResourceContext& context);

        ResStarlingAtlas();
        ~ResStarlingAtlas();

        void init(Resources*, pugi::xml_node& node);


        void mtload();

    private:
        void loadAtlas(CreateResourceContext& context);
        void _load(LoadResourcesContext*)override;
        void _unload() override;
        void _restore(Restorable* r, void*);

        spNativeTexture _texture;
        std::string _imagePath;
    };
}
