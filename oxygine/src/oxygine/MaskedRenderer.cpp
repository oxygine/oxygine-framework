#include "MaskedRenderer.h"
#include "core/VertexDeclaration.h"
#include "core/UberShaderProgram.h"
#include "RenderState.h"

namespace oxygine
{
    MaskedRenderer::MaskedRenderer(spNativeTexture mask,
                                   const RectF& srcRect, const RectF& destRect,
                                   const Transform& t, bool channelR, IVideoDriver* driver) : STDRenderer(driver)
    {
        ClipUV clipUV = ClipUV(
                            t.transform(destRect.getLeftTop()),
                            t.transform(destRect.getRightTop()),
                            t.transform(destRect.getLeftBottom()),
                            srcRect.getLeftTop(),
                            srcRect.getRightTop(),
                            srcRect.getLeftBottom());

        RectF clipMask = srcRect;
        Vector2 v(1.0f / mask->getWidth(), 1.0f / mask->getHeight());
        clipMask.expand(v, v);

        _baseShaderFlags |= UberShaderProgram::MASK;
        if (channelR)
            _baseShaderFlags |= UberShaderProgram::MASK_R_CHANNEL;

        clipUV.get(_msk);
        _clipMask = Vector4(clipMask.getLeft(), clipMask.getTop(), clipMask.getRight(), clipMask.getBottom());

        rsCache().setTexture(UberShaderProgram::SAMPLER_MASK, mask);
    }

    void MaskedRenderer::shaderProgramChanged()
    {
        _driver->setUniform("clip_mask", _clipMask);
        _driver->setUniform("msk", _msk, 4);
    }
}