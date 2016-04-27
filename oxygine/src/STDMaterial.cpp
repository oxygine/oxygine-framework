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

    void STDMaterial::setViewProj(const Matrix& view, const Matrix& proj)
    {
        _renderer->setViewProjTransform(view, proj);
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
        bool scissorEnabled = _renderer->getDriver()->getScissorRect(scissorRect);

        bool vis = true;
        if (actor->getClipping())
        {
            _renderer->drawBatch();

            RectF ss_rect = getActorTransformedDestRect(actor, actor->getTransform() * parentRS.transform);

            clippedRect.clip(ss_rect);
            if (!clippedRect.isEmpty())
            {
                Rect gl_rect = Rect(
                                   int(clippedRect.pos.x + 0.01f),
                                   int(clippedRect.pos.y + 0.01f),
                                   int(clippedRect.size.x + 0.01f),
                                   int(clippedRect.size.y + 0.01f));

                if (!_renderer->getDriver()->getRenderTarget()->getHandle())
                {
                    Point vp_size = core::getDisplaySize();
                    gl_rect.pos.y = vp_size.y - gl_rect.getBottom();
                }

                _renderer->getDriver()->setScissorRect(&gl_rect);
            }
            else
            {
                vis = false;
            }
        }

        if (vis)
            actor->_Actor::render(rs);


        if (actor->getClipping())
        {
            _renderer->drawBatch();

            _renderer->getDriver()->setScissorRect(scissorEnabled ? &scissorRect : 0);
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

            STDRenderer* original = _renderer;

            _renderer->drawBatch();

            MaskedRenderer mr(msk, maskSrc, maskDest, t, rchannel);
            mr.begin(_renderer);
            _renderer = &mr;
            RenderState rs = parentRS;
            sprite->_Sprite::render(rs);
            mr.end();

            _renderer = original;
        }
        else
        {
            sprite->_Sprite::render(parentRS);
        }
    }

    void STDMaterial::doRender(Sprite* sprite, const RenderState& rs)
    {
        Material::setCurrent(rs.material);

        const AnimationFrame& frame = sprite->getAnimFrame();
        const Diffuse& df = frame.getDiffuse();

        const spNativeTexture& base = df.base;


#ifdef EMSCRIPTEN
        _renderer->setTexture(df.base, df.alpha, df.premultiplied);//preload
        if (base && base->getHandle())
#else
        if (base)
#endif
        {
            _renderer->setBlendMode(sprite->getBlendMode());
#ifndef EMSCRIPTEN
            _renderer->setTexture(df.base, df.alpha, df.premultiplied);//preload
#endif

            _renderer->setTransform(rs.transform);
            _renderer->draw(rs.getFinalColor(sprite->getColor()), frame.getSrcRect(), frame.getDestRect());
        }
    }

    void STDMaterial::doRender(TextField* tf, const RenderState& rs)
    {
        text::Node* root = tf->getRootNode();
        if (!root)
            return;

        Material::setCurrent(this);



        text::DrawContext dc;

        dc.primary = rs.getFinalColor(tf->getColor()).premultiplied();
        dc.color = tf->getStyle().color * dc.primary;

        _renderer->setBlendMode(tf->getBlendMode());
        _renderer->setTransform(rs.transform);


        if (tf->getFont()->isSDF())
        {
            float scale = sqrtf(rs.transform.a * rs.transform.a + rs.transform.c * rs.transform.c);
            if (tf->getFontSize2Scale())
                scale = scale * tf->getFontSize2Scale() / tf->getFont()->getSize();

            float contrast = 3.0 + 30 * scale / 3.7f;
            float offset = 0.35f + 0.05f * scale / 3.7f;
            offset = 0.35f;


            if (scale < 1.0f)
            {
                contrast /= 2;
                offset = 0.25;
            }

            float outline = offset - tf->getOutline();
            _renderer->beginSDFont(contrast, offset, tf->getOutlineColor(), outline);
        }
        else
            _renderer->beginElementRendering(true);

        dc.renderer = _renderer;

        root->draw(dc);

        _renderer->endSDFont();
    }

    void STDMaterial::doRender(ColorRectSprite* sprite, const RenderState& rs)
    {
        Material::setCurrent(this);

        _renderer->setBlendMode(sprite->getBlendMode());
        _renderer->setTexture(STDRenderer::white);
        _renderer->setTransform(rs.transform);
        _renderer->draw(rs.getFinalColor(sprite->getColor()), RectF(0, 0, 1, 1), sprite->getDestRect());
    }

    void STDMaterial::doRender(ProgressBar*, const RenderState& rs)
    {

    }

    void STDMaterial::apply(Material* prev)
    {
        _renderer->begin(0);
        _renderer->setUberShaderProgram(&STDRenderer::uberShader);
    }

    void STDMaterial::finish()
    {
        if (!_renderer)
            return;
        _renderer->end();
    }
}