#pragma once
#include "oxygine-include.h"
#include "Actor.h"
#include "Box9Sprite.h"

namespace oxygine
{
    DECLARE_SMART(TexturesInspector, spTexturesInspector);
    class TexturesInspector: public Actor
    {
    public:
        TexturesInspector(const Vector2& size);
        ~TexturesInspector();
    };
}