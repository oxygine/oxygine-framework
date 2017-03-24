#include "STDMaterial.h"
#include "VisualStyle.h"
#include "Sprite.h"
#include "TextField.h"
#include "text_utils/Node.h"
#include "ColorRectSprite.h"
#include "RenderState.h"
#include "STDRenderer.h"
#include "MaskedRenderer.h"
#include "MaskedSprite.h"
#include "ClipRectActor.h"
#include "core/oxygine.h"
#include "res/ResFont.h"

namespace oxygine
{
    STDMaterial* STDMaterial::instance = 0;

    void Material::render(Actor* parent, const RenderState& parentRS)
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

    void STDMaterial::render(ClipRectActor* actor, const RenderState& parentRS)
    {
        //Material::render(actor, parentRS);
        //return;
        Material::setCurrent(this);

        RenderState rs = parentRS;

        const RectF* parentClippedRect = parentRS.clip;
        RectF clippedRect = *parentClippedRect;
        rs.clip = &clippedRect;

        Rect scissorRect(0, 0, 0, 0);
        STDRenderer* renderer = STDRenderer::getCurrent();
        IVideoDriver* driver = renderer->getDriver();
        bool scissorEnabled = driver->getScissorRect(scissorRect);

        bool vis = true;
        if (actor->getClipping())
        {
            renderer->drawBatch();

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
            renderer->drawBatch();
            driver->setScissorRect(scissorEnabled ? &scissorRect : 0);
        }
    }

    void STDMaterial::render(MaskedSprite* sprite, const RenderState& parentRS)
    {
        Material::setCurrent(this);


        spSprite mask = sprite->getMask();
        if (mask && mask->getAnimFrame().getDiffuse().base)
        {
            Transform t = mask->computeGlobalTransform();

            RectF maskDest = mask->getDestRect();
            RectF maskSrc = mask->getSrcRect();

            const Diffuse& df = mask->getAnimFrame().getDiffuse();

            bool useR = sprite->getUseRChannel();
            bool rchannel       = useR ? true    : (df.alpha ? true     : false);
            spNativeTexture msk = useR ? df.base : (df.alpha ? df.alpha : df.base);

            STDRenderer* original = STDRenderer::getCurrent();

            original->drawBatch();
            original->end();



            MaskedRenderer mr(msk, maskSrc, maskDest, t, rchannel, original->getDriver());
            original->swapVerticesData(mr);

            mr.setViewProj(original->getViewProjection());
            mr.begin();

            RenderState rs = parentRS;
            sprite->Sprite::render(rs);
            mr.end();

            original->swapVerticesData(mr);
            original->begin();
        }
        else
        {
            sprite->Sprite::render(parentRS);
        }
    }

    void STDMaterial::doRender(Sprite* sprite, const RenderState& rs)
    {
        if (!sprite->getAnimFrame().getDiffuse().base)
            return;

        Color color = rs.getFinalColor(sprite->getColor());

        sprite->_mat->render(rs.transform, color, sprite->getAnimFrame().getSrcRect(), sprite->getDestRect());
    }

    void STDMaterial::doRender(TextField* tf, const RenderState& rs)
    {
        float scale = sqrtf(rs.transform.a * rs.transform.a + rs.transform.c * rs.transform.c);

        text::Node* root = tf->getRootNode(scale);
        if (!root)
            return;


        text::DrawContext dc;

        STDRenderer* renderer = STDRenderer::getCurrent();

        dc.primary = rs.getFinalColor(tf->getColor()).premultiplied();
        dc.color = tf->getStyle().color * dc.primary;
        dc.renderer = renderer;

        //renderer->setBlendMode(tf->getBlendMode());
        renderer->setTransform(rs.transform);

        int sdfSize;
        if (tf->getFont()->isSDF(sdfSize))
        {
            if (tf->getFontSize())
                scale = scale * tf->getFontSize() / sdfSize;

            float contrast = 3.0f + scale * 8.0f;
            float offset = tf->getWeight();
            float outline = tf->getWeight() - tf->getOutline();

            renderer->applySDF(contrast, offset, tf->getOutlineColor(), outline);
            root->draw(dc);
            renderer->endSDF();
        }
        else
        {
            renderer->applySimpleMode(true);
            root->draw(dc);
        }
    }

    void STDMaterial::doRender(ColorRectSprite* sprite, const RenderState& rs)
    {
        Color color = rs.getFinalColor(sprite->getColor());

        sprite->_mat->render(rs.transform, color, sprite->getAnimFrame().getSrcRect(), sprite->getDestRect());
    }

    void STDMaterial::doRender(ProgressBar*, const RenderState& rs)
    {

    }

    void STDMaterial::apply(Material* prev)
    {
        STDRenderer* cur = STDRenderer::getCurrent();
        cur->begin();
        cur->setUberShaderProgram(&STDRenderer::uberShader);
    }

    void STDMaterial::finish()
    {
        STDRenderer::getCurrent()->end();
    }
}