#include "TweenAlphaFade.h"
#include "core/file.h"

namespace oxygine
{
    ShaderProgram* TweenGlow::shaderBlurV = 0;
    ShaderProgram* TweenGlow::shaderBlurH = 0;


    void TweenGlow::render2texture()
    {
        if (!shaderBlurH)
        {
            file::buffer vs_h;
            file::buffer vs_v;
            file::buffer fs;
            file::read("pp_hblur_vs.glsl", vs_h);
            file::read("pp_vblur_vs.glsl", vs_v);
            file::read("pp_rast_vs.glsl", fs);

        }
    }
}