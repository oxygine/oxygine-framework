#pragma once
#include "../oxygine-include.h"
#include "../actor/Actor.h"
#include "../actor/Box9Sprite.h"

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