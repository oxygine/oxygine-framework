#pragma once
#include "oxygine-include.h"

namespace oxygine
{
    class Material
    {
    public:
        virtual ~Material();
        virtual void apply(Material* prev = 0) {}
        virtual void finish() {}

        virtual void render(Actor*, const RenderState& rs);
        virtual void render(ClipRectActor*, const RenderState& rs) {}
        virtual void render(MaskedSprite*, const RenderState& rs) {}
        virtual void doRender(Sprite*, const RenderState& rs) {}
        virtual void doRender(TextField*, const RenderState& rs) {}
        virtual void doRender(ColorRectSprite*, const RenderState& rs) {}
        virtual void doRender(ProgressBar*, const RenderState& rs) {}
    };
}