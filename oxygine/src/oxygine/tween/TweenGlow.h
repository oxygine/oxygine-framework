#pragma once
#include "oxygine-include.h"
#include "../PostProcess.h"

namespace oxygine
{
    class TweenGlow : public TweenProxy
    {
    public:
        TweenGlow(const Color& color, const PostProcessOptions& opt = PostProcessOptions());
    };
}