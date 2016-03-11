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
        TextureFormat _format;

        Actor* _actor;
        Material* _prev;

        spNativeTexture _rt;
        Transform _rtTransform;
        Rect _screen;
        enum options
        {
            opt_singleR2T = 1,
            opt_fullscreen = 1 << 1,
        };

        int _options;

        TweenPostProcess(int opt);
        ~TweenPostProcess();
        Rect getScreenRect(const Actor& actor) const;
        void init(Actor& actor);
        void restore(Restorable* r, void* userData);
        void done(Actor& actor);
        bool begin();
        virtual void rtCreated();
        virtual void render2texture();
        void update(Actor& actor, float p, const UpdateState& us);
        void apply(Material* prev);
    };

    class TweenAlphaFade : public TweenPostProcess
    {
    public:
        unsigned char _a;
        bool _fadeIn;

        TweenAlphaFade(bool fadeIn, int opt = 0);

        void update(Actor& actor, float p, const UpdateState& us);
        void render(Actor* actor, const RenderState& rs);
    };

    class TweenGlow : public TweenPostProcess
    {
    public:
        static ShaderProgram* shaderBlurV;
        static ShaderProgram* shaderBlurH;
        static ShaderProgram* shaderBlit;

        int _downsample;
        spNativeTexture _rt2;

        TweenGlow(): TweenPostProcess(0), _downsample(1) {}

        void render2texture() OVERRIDE;
        void rtCreated() OVERRIDE;


        void render(Actor* actor, const RenderState& rs)
        {
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();

            RectF src(0, 0,
                      _screen.getWidth() / (float)_rt->getWidth() / _downsample,
                      _screen.getHeight() / (float)_rt->getHeight() / _downsample);
            RectF dest = _screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);

            AffineTransform tr;
            tr.identity();
            renderer->setTransform(tr);
            renderer->beginElementRendering(true);
            Color color = Color(Color::White).withAlpha(255).premultiplied();
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