#include "TweenAlphaFade.h"
#include "STDMaterial.h"
#include "Actor.h"

namespace oxygine
{
    class TweenAlphaFadeImpl : public TweenPostProcess
    {
    public:
        TweenAlphaFadeImpl(bool fadeIn, const PostProcessOptions& opt) : TweenPostProcess(opt), _fadeIn(fadeIn) {}

        void render(Actor* actor, const RenderState& rs)
        {
            if (!_pp._rt)
                return;

            int _a = lerp(_fadeIn ? 0 : 255, _fadeIn ? 255 : 0, _progress);
            STDRenderer* renderer = STDRenderer::getCurrent();

            spNativeTexture rt = _pp._rt;
            RectF src(0, 0,
                      _pp._screen.getWidth() / (float)rt->getWidth(),
                      _pp._screen.getHeight() / (float)rt->getHeight());
            RectF dest = _pp._screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);
            AffineTransform tr = _pp._transform * _actor->computeGlobalTransform();

            renderer->setTransform(tr);
            renderer->applySimpleMode(true);
            Color color = Color(Color::White).withAlpha(_a).premultiplied();
            renderer->draw(rt, color.rgba(), src, dest);
            renderer->drawBatch();
        }

        bool _fadeIn;
    };


    TweenAlphaFade::TweenAlphaFade(bool fadeIn, const PostProcessOptions& opt): TweenProxy(new TweenAlphaFadeImpl(fadeIn, opt))
    {
    }
}