#include "STDRenderDelegate.h"
#include "MaskedRenderer.h"
#include "RenderState.h"
#include "STDRenderer.h"
#include "VisualStyle.h"
#include "actor/ClipRectActor.h"
#include "actor/ColorRectSprite.h"
#include "actor/MaskedSprite.h"
#include "actor/Sprite.h"
#include "actor/TextField.h"
#include "core/NativeTexture.h"
#include "core/UberShaderProgram.h"
#include "core/oxygine.h"
#include "res/ResFont.h"
#include "text_utils/Node.h"

namespace oxygine
{
    STDRenderDelegate* STDRenderDelegate::instance = 0;

    void RenderDelegate::render(Actor* parent, const RenderState& parentRS)
    {
        RenderState rs;
        if (!parent->internalRender(rs, parentRS))
            return;

        Actor* actor = parent->getFirstChild().get();
        while (actor)
        {
            OX_ASSERT(actor->getParent());
            actor->render(rs);
            actor = actor->getNextSibling().get();
        }
    }

    void STDRenderDelegate::render(ClipRectActor* actor, const RenderState& parentRS)
    {
        STDRenderer* renderer = STDRenderer::getCurrent();
        IVideoDriver* driver = renderer->getDriver();

        RenderState rs = parentRS;

        const RectF* parentClippedRect = parentRS.clip;
        RectF clippedRect = *parentClippedRect;
        rs.clip = &clippedRect;

        Rect scissorRect(0, 0, 0, 0);


        bool scissorEnabled = driver->getScissorRect(scissorRect);

        bool vis = true;
        if (actor->getClipping())
        {
            renderer->flush();

            RectF ss_rect = getActorTransformedDestRect(actor, actor->getTransform() * parentRS.transform);

            clippedRect.clip(ss_rect);
            if (!clippedRect.isEmpty())
            {
                Rect gl_rect = Rect(
                                   int(clippedRect.pos.x + 0.01f),
                                   int(clippedRect.pos.y + 0.01f),
                                   int(clippedRect.size.x + 0.01f),
                                   int(clippedRect.size.y + 0.01f));

                if (!driver->getRenderTarget()->getHandle())
                {
                    Point vp_size = core::getDisplaySize();
                    gl_rect.pos.y = vp_size.y - gl_rect.getBottom();
                }

                driver->setScissorRect(&gl_rect);
            }
            else
            {
                vis = false;
            }
        }

        if (vis)
            actor->Actor::render(rs);


        if (actor->getClipping())
        {
            renderer->flush();
            driver->setScissorRect(scissorEnabled ? &scissorRect : 0);
        }
    }

    void STDRenderDelegate::render(MaskedSprite* sprite, const RenderState& parentRS)
    {
        spSprite maskSprite = sprite->getMask();
        if (!maskSprite)
        {
            sprite->Sprite::render(parentRS);
            return;
        }

        const Diffuse& df = maskSprite->getAnimFrame().getDiffuse();
        if (!df.base)
        {
            sprite->Sprite::render(parentRS);
            return;
        }


        Material::null->apply();

        Transform world = maskSprite->computeGlobalTransform();

        RectF maskDest = maskSprite->getDestRect();
        RectF maskSrc = maskSprite->getSrcRect();


        bool useR           = sprite->getUseRChannel();
        bool rchannel               = useR ? true    : (df.alpha ? true     : false);
        spNativeTexture maskTexture = useR ? df.base : (df.alpha ? df.alpha : df.base);

        STDRenderer* renderer = STDRenderer::getCurrent();



#if 1
        ClipUV clipUV = ClipUV(
                            world.transform(maskDest.getLeftTop()),
                            world.transform(maskDest.getRightTop()),
                            world.transform(maskDest.getLeftBottom()),
                            maskSrc.getLeftTop(),
                            maskSrc.getRightTop(),
                            maskSrc.getLeftBottom());

        Vector2 v(1.0f / maskTexture->getWidth(), 1.0f / maskTexture->getHeight());
        maskSrc.expand(v, v);



        int sflags = renderer->getBaseShaderFlags();
        int baseShaderFlags = sflags;

        baseShaderFlags |= UberShaderProgram::MASK;
        if (rchannel)
            baseShaderFlags |= UberShaderProgram::MASK_R_CHANNEL;

        Vector3 msk[4];

        clipUV.get(msk);
        Vector4 clipMask = Vector4(maskSrc.getLeft(), maskSrc.getTop(), maskSrc.getRight(), maskSrc.getBottom());

        rsCache().setTexture(UberShaderProgram::SAMPLER_MASK, maskTexture);



        ShaderProgramChangedHook hook;
        hook.hook = [&]()
        {

            IVideoDriver::instance->setUniform("clip_mask", clipMask);
            IVideoDriver::instance->setUniform("msk", msk, 4);

        };


        renderer->pushShaderSetHook(&hook);
        renderer->setBaseShaderFlags(baseShaderFlags);

        sprite->Sprite::render(parentRS);

        Material::null->apply();

        renderer->popShaderSetHook();
        renderer->setBaseShaderFlags(sflags);
#else

        MaskedRenderer mr(maskTexture, maskSrc, maskDest, world, rchannel, renderer->getDriver());
        renderer->swapVerticesData(mr);

        mr.setViewProj(renderer->getViewProjection());
        mr.begin();

        RenderState rs = parentRS;
        sprite->Sprite::render(rs);
        mr.end();

        Material::null->apply();

        renderer->swapVerticesData(mr);
        renderer->begin();
#endif

    }

    void STDRenderDelegate::doRender(Sprite* sprite, const RenderState& rs)
    {
        if (!sprite->getAnimFrame().getDiffuse().base)
            return;


        Color color = rs.getFinalColor(sprite->getColor());

        sprite->_mat->apply();
        sprite->_mat->render(rs.transform, color, sprite->getAnimFrame().getSrcRect(), sprite->getDestRect());
    }

    void STDRenderDelegate::doRender(TextField* tf, const RenderState& rs)
    {
        float scale = sqrtf(rs.transform.a * rs.transform.a + rs.transform.c * rs.transform.c);

        text::Node* root = tf->getRootNode(scale);
        if (!root)
            return;


        text::DrawContext dc;

        STDRenderer* renderer = STDRenderer::getCurrent();

        dc.primary = rs.getFinalColor(tf->getColor()).premultiplied();
        dc.color = tf->getStyle().color * dc.primary;

        //renderer->setBlendMode(tf->getBlendMode());
        renderer->setTransform(rs.transform);
        root->draw(dc);
    }

    void STDRenderDelegate::doRender(ColorRectSprite* sprite, const RenderState& rs)
    {
        sprite->_mat->apply();
        Color color = rs.getFinalColor(sprite->getColor());
        sprite->_mat->render(rs.transform, color, sprite->getAnimFrame().getSrcRect(), sprite->getDestRect());
    }

    void STDRenderDelegate::doRender(ProgressBar*, const RenderState& rs)
    {

    }
}