#include "ResFont.h"
namespace oxygine
{

    void ResFont::alignSize(float worldScale, int styleFontSize, float& resScale, int& resFontSize) const
    {
        resFontSize = styleFontSize;
        resScale = worldScale;

        /*
        if (fontSize)
            resScale = _font->getSize() / float(fontSize);
        else
        {
            resFontSize = _font->getScale();
            resScale = _font->getScale();
        }*/
    }

}