#pragma once
#include "oxygine_include.h"
#include "Tween.h"
#include "STDMaterial.h"
#include "core/NativeTexture.h"


namespace oxygine
{

    class TweenAlphaFade: public Material
    {
    public:
        typedef Actor type;

        Actor* _actor;
        Material* _prev;
        spNativeTexture _rt;
        unsigned char _a;
        bool _fadeIn;

        enum options
        {
            opt_singleR2T = 1,
            opt_fullscreen = 1 << 1,
        };

        int _options;

        TweenAlphaFade(bool fadeIn, int opt = opt_singleR2T) : _actor(0), _prev(0), _fadeIn(fadeIn), _options(opt) {}

        ~TweenAlphaFade()
        {
            if (_actor)
                _actor->setMaterial(0);
            if (_rt)
                _rt->release();
            _rt = 0;
        }


        Rect getScreenRect(Actor& actor)
        {
            RectF screen;
            screen.pos = actor._getStage()->local2global(convert_local2stage(&actor, Vector2(0, 0)));
            screen.size = actor._getStage()->local2global(convert_local2stage(&actor, actor.getSize())) - screen.pos;
            screen.size += Vector2(1, 1);

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
            Rect screen = getScreenRect(*_actor);
            _rt = IVideoDriver::instance->createTexture();
            _rt->init(screen.getWidth(), screen.getHeight(), TF_R8G8B8A8, true);

            _rt->reg(CLOSURE(this, &TweenAlphaFade::restore), 0);


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

        void render2texture()
        {
            if (!STDRenderer::isReady())
                return;

            Material::setCurrent(0);
            IVideoDriver* driver = IVideoDriver::instance;



            Actor& actor = *_actor;
            Rect screen = getScreenRect(actor);

            if (_options & opt_fullscreen)
            {
                //driver->getViewport(screen);
                screen.pos = Point(0, 0);
                screen.size = core::getDisplaySize();
            }


            if (_rt->getWidth() < screen.getWidth() ||
                    _rt->getHeight() < screen.getHeight())
                _rt->init(screen.getWidth(), screen.getHeight(), TF_R8G8B8A8, true);

            driver->setRenderTarget(_rt);

            Rect vp = screen;
            vp.pos = Point(0, 0);
            driver->setViewport(vp);
            driver->clear(0);


            RenderState rs;
            Material* mat = STDMaterial::instance;
            STDRenderer* renderer = STDMaterial::instance->getRenderer();
            rs.material = mat;

            RectF clip = vp.cast<RectF>();
            rs.clip = &clip;

            renderer->initCoordinateSystem(vp.getWidth(), vp.getHeight(), true);

            if (_options & opt_fullscreen)
            {
                rs.transform = getGlobalTransform(_actor->getParent());
                mat->render(&actor, rs);
            }
            else
            {
                AffineTransform copy = actor.getTransform();

                AffineTransform transform;
                transform.identity();
                Vector2 gpos = actor._getStage()->local2global(convert_local2stage(&actor, Vector2(0, 0)));
                Vector2 offset = gpos - screen.pos.cast<Vector2>();
                transform.translate(offset);
                transform.scale(actor.getScale());
                actor.setTransform(transform);

                mat->render(&actor, rs);

                //restore original transform
                actor.setTransform(copy);
            }

            mat->finish();
            driver->setRenderTarget(0);
        }

        void update(Actor& actor, float p, const UpdateState& us)
        {
            if (!(_options & opt_singleR2T))
                render2texture();
            _a = lerp(_fadeIn ? 0 : 255, _fadeIn ? 255 : 0, p);
        }

        void apply(Material* prev)
        {

        }

        void render(Actor* actor, const RenderState& rs)
        {
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();
            Rect sr = getScreenRect(*actor);
            if (_options & opt_fullscreen)
            {
                sr.pos = Point(0, 0);
                sr.size = core::getDisplaySize();
            }
            RectF src(0, 0,
                      sr.getWidth()  / (float)_rt->getWidth(),
                      sr.getHeight() / (float)_rt->getHeight());
            RectF dest = sr.cast<RectF>();

            //renderer->drawBatch();
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
}