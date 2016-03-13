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
        enum
        {
            flag_singleR2T = 1,
            flag_fullscreen = 1 << 1,
        };

        PostProcessOptions(int flags = 0): _flags(flags), _downscale(1) {}
        PostProcessOptions& fullscreen(bool enable = true) { _flags = enable ? (_flags | flag_fullscreen) : (_flags  & (~flag_fullscreen)); return *this; }
        PostProcessOptions& singleRender(bool enable = true) { _flags = enable ? (_flags | flag_singleR2T) : (_flags  & (~flag_singleR2T)); return *this; }
        //loops -(2, 3, 4, ...),  final size: 2^loops
        PostProcessOptions& downscale(int loops = 2) { _downscale = loops; return *this; }

        int _flags;
        int _downscale;
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