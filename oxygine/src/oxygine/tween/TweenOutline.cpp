#include "TweenOutline.h"
#include "../RenderState.h"
#include "../STDRenderDelegate.h"
#include "../actor/Actor.h"
#include "../core/gl/VertexDeclarationGL.h"

namespace oxygine
{
    class TweenOutlineImpl : public TweenPostProcess
    {
    public:
        Color _color;
        int _downsample;
        spSTDMaterial _matx;

        TweenOutlineImpl(const Color& c, const PostProcessOptions& opt) : TweenPostProcess(opt), _color(c), _downsample(1)
        {
            _matx = new STDMaterial;
            _matx->_blend = blend_premultiplied_alpha;
        }

        void render(Actor* actor, const RenderState& rs) override
        {
            if (!_pp._rt)
                return;

            spNativeTexture rt = _pp._rt;

            _matx->_base = rt;
            _matx->apply();

            STDRenderer* renderer = STDRenderer::getCurrent();


            RectF src(0, 0,
                      _pp._screen.getWidth() / (float)rt->getWidth(),
                      _pp._screen.getHeight() / (float)rt->getHeight());
            RectF dest = _pp._screen.cast<RectF>();


            AffineTransform tr = _pp._transform * _actor->computeGlobalTransform();
            renderer->setTransform(tr);


            Color color = Color(Color::White).withAlpha(255).premultiplied();
            renderer->addQuad(color.rgba(), src, dest);


            RenderState r = rs;
            actor->setRenderDelegate(_prevMaterial);
            actor->render(r);
            actor->setRenderDelegate(this);
        }

        void _renderPP() override
        {
            PostProcess::initShaders();

            int w = _pp._screen.size.x;
            int h = _pp._screen.size.y;

            if (w < 0 || h < 0)
                return;



            IVideoDriver* driver = IVideoDriver::instance;
            const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2::FORMAT));

            _downsample = 1;


            spNativeTexture rt = _pp._rt;
            spNativeTexture rt2 = getRTManager().get(0, w, h, _pp._format);

#if 0
            driver->setShaderProgram(PostProcess::shaderBlit);
            pass(rt, Rect(0, 0, w, h), rt2, Rect(0, 0, w / 2, h / 2));

            w /= 2;
            h /= 2;
            _downsample *= 2;
#endif

#if 0

            rt = getRTManager().get(w / 2, h / 2, _pp._format);
            _pp._rt = rt;
            pass(rt2, Rect(0, 0, w, h), rt, Rect(0, 0, w / 2, h / 2));

            w /= 2;
            h /= 2;
            _downsample *= 2;
#endif


            Rect rc(0, 0, w, h);


            driver->setShaderProgram(PostProcess::shaderBlurH);
            driver->setUniform("step", 1.0f / rt->getWidth());
            pass(rt, rc, rt2, rc);


            int alpha = lerp(0, 255, _progress);
            //log::messageln("tween alpha %d", alpha);

            Color c;
            if (_pp._options._flags & PostProcessOptions::flag_singleR2T)
                c = _color;
            else
                c = _color.withAlpha(alpha).premultiplied();

            driver->setShaderProgram(PostProcess::shaderBlurV);
            driver->setUniform("step", 1.0f / rt2->getHeight());

            pass(rt2, rc, rt, rc, c);
        }
    };


    TweenOutline::TweenOutline(const Color& color, const PostProcessOptions& opt) : TweenProxy(new TweenOutlineImpl(color, opt))
    {
    }
}