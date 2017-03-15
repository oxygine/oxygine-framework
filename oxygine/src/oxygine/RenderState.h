#pragma once
#include "oxygine-include.h"
#include "core/Renderer.h"
#include "STDMaterial.h"

namespace oxygine
{
    class RState
    {
    public:
        RState(): alpha(255), clip(0)
        {
            transform.identity();
        }

        Color getFinalColor(const Color& clr) const
        {
            Color color = clr;
            color = clr.withAlpha((int(color.a) * alpha) / 255);
            return color;
        }

        Transform transform;
        unsigned char alpha;
        const RectF* clip;
    };

    class RenderState: public RState
    {
    public:
        RenderState(Material* mat = STDMaterial::instance): material(mat) {}
        Material* material;
    };
}