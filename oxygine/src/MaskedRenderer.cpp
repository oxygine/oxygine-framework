#include "MaskedRenderer.h"
#include "core/VertexDeclaration.h"
#include "core/UberShaderProgram.h"
#include "RenderState.h"

namespace oxygine
{
    MaskedRenderer::MaskedRenderer(spNativeTexture mask, const RectF& srcRect, const RectF& destRect, const Transform& t, bool channelR, IVideoDriver* driver) : STDRenderer(driver), _mask(mask)
    {
        _clipUV = ClipUV(
                      t.transform(destRect.getLeftTop()),
                      t.transform(destRect.getRightTop()),
                      t.transform(destRect.getLeftBottom()),
                      srcRect.getLeftTop(),
                      srcRect.getRightTop(),
                      srcRect.getLeftBottom());

        _clipMask = srcRect;
        Vector2 v(1.0f / mask->getWidth(), 1.0f / mask->getHeight());
        _clipMask.expand(v, v);

        _shaderFlags |= UberShaderProgram::MASK;
        if (channelR)
            _shaderFlags |= UberShaderProgram::MASK_R_CHANNEL;
    }

    void MaskedRenderer::preDrawBatch()
    {
        STDRenderer::preDrawBatch();

        _driver->setTexture(UberShaderProgram::SAMPLER_MASK, _mask);

        Vector4 v(_clipMask.getLeft(), _clipMask.getTop(), _clipMask.getRight(), _clipMask.getBottom());
        _driver->setUniform("clip_mask", &v, 1);

        Vector3 msk[4];
        _clipUV.get(msk);

        _driver->setUniform("msk", msk, 4);
    }
}