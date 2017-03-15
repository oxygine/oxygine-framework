#pragma once
#include "../oxygine-include.h"
#include "Resource.h"
#include "../core/file.h"
#include <vector>

namespace oxygine
{
    class ResBuffer: public Resource
    {
        INHERITED(Resource);
    public:
        static Resource* create(CreateResourceContext&);

        ResBuffer();
        ~ResBuffer();

        void init(const char* file);

        const file::buffer& getBuffer() const {return _buffer;}

    private:
        void _load(LoadResourcesContext*) override;
        void _unload() override;

        file::buffer _buffer;
        std::string _path;
    };
}
