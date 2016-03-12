#pragma once
#include "oxygine_include.h"
#include "Tween.h"
#include "STDMaterial.h"
#include "core/NativeTexture.h"
#include "Actor.h"
#include "core/oxygine.h"
#include "RenderState.h"

namespace oxygine
{
    class PostProcessOptions
    {
    public:
        enum flags
        {
            singleR2T = 1,
            fullscreen = 1 << 1,
        };

        int _flags;
    };

    class TweenOutline : public TweenProxy
    {
    public:
        TweenOutline(const Color& color, const PostProcessOptions& opt = PostProcessOptions());
    };

    class TweenAlphaFade: public TweenProxy
    {
    public:
        TweenAlphaFade(bool fadeIn, const PostProcessOptions& opt = PostProcessOptions());
    };
}