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
    class PostProcessOptions
    {
    public:
        enum flags
        {
            singleR2T = 1,
            fullscreen = 1 << 1,
        };

        int _flags;
    };

    class TweenOutline : public TweenProxy
    {
    public:
        TweenOutline(const Color& color, const PostProcessOptions& opt = PostProcessOptions());
    };

    class TweenAlphaFade: public TweenProxy
    {
    public:
        TweenAlphaFade(bool fadeIn, const PostProcessOptions& opt = PostProcessOptions());
    };

    /*
    class TweenPostProcess: public Tween, public Material
    {
    public:
        typedef Actor type;

        PostProcess _pp;

        Actor* _actor;
        Material* _prev;
        float _progress;


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

    class TweenOutline : public TweenPostProcess
    {
    public:
        static ShaderProgram* shaderBlurV;
        static ShaderProgram* shaderBlurH;
        static ShaderProgram* shaderBlit;

        int _downsample;
        spNativeTexture _rt2;
        Color _color;

        TweenOutline(const Color& color): TweenPostProcess(0), _downsample(1), _color(color) {}

        void render2texture() OVERRIDE;
        void rtCreated() OVERRIDE;


        void render(Actor* actor, const RenderState& rs)
        {
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();

            RectF src(0, 0,
                      _pp._screen.getWidth() / (float)_pp._rt->getWidth() / _downsample,
                        _pp._screen.getHeight() / (float)_pp._rt->getHeight() / _downsample);
            RectF dest = _pp._screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);


            AffineTransform tr = _pp._rtTransform * _actor->computeGlobalTransform();
            renderer->setTransform(tr);
            renderer->beginElementRendering(true);
            Color color = Color(Color::White).withAlpha(255).premultiplied();
            renderer->drawElement(_pp._rt, color.rgba(), src, dest);
            renderer->drawBatch();


            RenderState r = rs;
            r.material = mat;
            actor->setMaterial(_prev);
            actor->render(r);
            actor->setMaterial(this);
        }
    };*/
}