#pragma once
#include "oxygine_include.h"
#include "Resource.h"
namespace oxygine
{
    class Font;

    DECLARE_SMART(ResFont, spResFont);
    class ResFont: public Resource
    {
        INHERITED(Resource);
    public:
        ResFont(): _size(0) {}

        virtual const Font* getFont(const char* name = 0, int size = 0) const = 0;
        virtual bool isSDF(int& size) const { size = 0; return false; }
        int getSize() const { return _size; }

    protected:
        int _size;
    };
}
