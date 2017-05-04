#pragma once
#include "oxygine-include.h"
#include "../PostProcess.h"

namespace oxygine
{
    class TweenOutline : public TweenProxy
    {
    public:
        TweenOutline(const Color& color, const PostProcessOptions& opt = PostProcessOptions());
    };
}