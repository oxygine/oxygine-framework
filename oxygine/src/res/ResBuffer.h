#pragma once
#include "oxygine_include.h"
#include "Resource.h"
#include "core/file.h"
#include <vector>

namespace oxygine
{
    class Resources;
    class CreateResourceContext;

    class ResBuffer: public _Resource
    {
    public:
        static Resource* create(CreateResourceContext&);

        ResBuffer();
        ~ResBuffer();

        void init(const char* file);

        const file::buffer& getBuffer() const {return _buffer;}

    private:
        void _load(LoadResourcesContext*);
        void _unload();

        file::buffer _buffer;
        std::string _path;
    };
}
