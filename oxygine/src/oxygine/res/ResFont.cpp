#include "ResFont.h"

namespace oxygine
{
    const oxygine::Font* ResFont::getClosestFont(float worldScale, int styleFontSize, float& resScale) const
    {
        resScale = 1.0f;
        return getFont(0, styleFontSize);
    }
}