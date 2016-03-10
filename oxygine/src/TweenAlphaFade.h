#pragma once
#include "oxygine_include.h"
#include "Tween.h"
#include "STDMaterial.h"
#include "core/NativeTexture.h"
#include "Actor.h"
#include "core/oxygine.h"
#include "RenderState.h"

namespace oxygine
{
    class TweenPostProcess: public Material
    {
    public:
        typedef Actor type;

        Actor* _actor;
        Material* _prev;
        spNativeTexture _rt;

        Rect _screen;
        enum options
        {
            opt_singleR2T = 1,
            opt_fullscreen = 1 << 1,
        };

        int _options;

        TweenPostProcess(int opt) : _actor(0), _prev(0), _options(opt) {}

        ~TweenPostProcess()
        {
            if (_actor)
                _actor->setMaterial(0);
            if (_rt)
                _rt->release();
            _rt = 0;
        }


        Rect getScreenRect(const Actor& actor) const
        {
            Rect screen;

            Rect display(Point(0, 0), core::getDisplaySize());

            if (_options & opt_fullscreen)
                return display;

            screen = actor.computeBounds(actor.computeGlobalTransform()).cast<Rect>();
            screen.size += Point(1, 1);
            screen.expand(Point(18, 18), Point(18, 18));
            screen.clip(display);

            return screen.cast<Rect>();
        }

        void init(Actor& actor)
        {
            _actor = &actor;
            _prev = _actor->getMaterial();
            actor.setMaterial(this);

            restore(0, 0);

        }

        void restore(Restorable* r, void* userData)
        {
            render2texture();
        }

        void done(Actor& actor)
        {
            actor.setMaterial(0);
            if (_rt)
                _rt->release();
            _rt = 0;
            _actor = 0;
        }

        bool begin()
        {
            if (!STDRenderer::isReady())
                return false;

            Material::setCurrent(0);
            IVideoDriver* driver = IVideoDriver::instance;


            Rect display(Point(0, 0), core::getDisplaySize());

            Actor& actor = *_actor;
            _screen = getScreenRect(actor);

            bool c = false;
            if (!_rt)
            {
                _rt = IVideoDriver::instance->createTexture();
                _rt->init(_screen.getWidth(), _screen.getHeight(), TF_R8G8B8A8, true);
                c = true;
            }

            if (_rt->getWidth() < _screen.getWidth() || _rt->getHeight() < _screen.getHeight())
            {
                _rt->init(_screen.getWidth(), _screen.getHeight(), TF_R8G8B8A8, true);
                c = true;
            }

            if (c)
                rtCreated();

            return true;
        }

        virtual void rtCreated()
        {
            _rt->reg(CLOSURE(this, &TweenPostProcess::restore), 0);
        }

        virtual void render2texture()
        {
            if (!begin())
                return;

            IVideoDriver* driver = IVideoDriver::instance;
            driver->setRenderTarget(_rt);

            Rect vp = _screen;
            vp.pos = Point(0, 0);
            driver->setViewport(vp);
            driver->clear(0);


            RenderState rs;
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();
            rs.material = mat;

            RectF clip = vp.cast<RectF>();
            rs.clip = &clip;

            renderer->initCoordinateSystem(vp.getWidth(), vp.getHeight(), true);

            rs.transform = _actor->getParent()->computeGlobalTransform();
            if (!(_options & opt_fullscreen))
            {
                AffineTransform offset;
                offset.identity();
                offset.translate(-_screen.pos);
                rs.transform = rs.transform * offset;
            }

            mat->Material::render(_actor, rs);

            mat->finish();
            driver->setRenderTarget(0);
        }

        void update(Actor& actor, float p, const UpdateState& us)
        {
            if (!(_options & opt_singleR2T))
                render2texture();
        }

        void apply(Material* prev)
        {

        }
    };

    class TweenAlphaFade : public TweenPostProcess
    {
    public:
        unsigned char _a;
        bool _fadeIn;

        TweenAlphaFade(bool fadeIn, int opt = 0) : TweenPostProcess(opt), _fadeIn(fadeIn), _a(0) {}

        void update(Actor& actor, float p, const UpdateState& us)
        {
            TweenPostProcess::update(actor, p, us);
            _a = lerp(_fadeIn ? 0 : 255, _fadeIn ? 255 : 0, p);
        }

        void render(Actor* actor, const RenderState& rs)
        {
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();

            RectF src(0, 0,
                      _screen.getWidth() / (float)_rt->getWidth(),
                      _screen.getHeight() / (float)_rt->getHeight());
            RectF dest = _screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);
            AffineTransform tr;
            tr.identity();
            renderer->setTransform(tr);
            renderer->beginElementRendering(true);
            Color color = Color(Color::White).withAlpha(_a).premultiplied();
            renderer->drawElement(_rt, color.rgba(), src, dest);
            renderer->drawBatch();
        }
    };

    class TweenGlow : public TweenPostProcess
    {
    public:
        static ShaderProgram* shaderBlurV;
        static ShaderProgram* shaderBlurH;
        static ShaderProgram* shaderBlit;

        spNativeTexture _rt2;

        TweenGlow(): TweenPostProcess(0) {}

        void render2texture() OVERRIDE;
        void rtCreated() OVERRIDE;


        void render(Actor* actor, const RenderState& rs)
        {
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();

            RectF src(0, 0,
                      _screen.getWidth() / (float)_rt->getWidth() / 4,
                      _screen.getHeight() / (float)_rt->getHeight() / 4);
            RectF dest = _screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);
            AffineTransform tr;
            tr.identity();
            //tr.scale(Vector2(4, 4));
            renderer->setTransform(tr);
            renderer->beginElementRendering(true);
            Color color = Color(Color::White).premultiplied();
            renderer->drawElement(_rt, color.rgba(), src, dest);
            renderer->drawBatch();


            RenderState r = rs;
            r.material = mat;
            actor->setMaterial(_prev);
            actor->render(r);
            actor->setMaterial(this);

        }
    };
}