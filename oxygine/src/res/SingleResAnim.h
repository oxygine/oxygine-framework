#pragma once
#include "oxygine-include.h"
#include "ResAnim.h"

namespace oxygine
{
    class SingleResAnim: public ResAnim
    {
    public:
        void init(const std::string& file, int columns = 1, int rows = 1, float scaleFactor = 1.0f);
        void init(Image* original, int columns = 1, int rows = 1, float scaleFactor = 1.0f) OVERRIDE;

        spNativeTexture getTexture() const;

    protected:
        std::vector<unsigned char> _data;
        spNativeTexture _texture;
    };
}
