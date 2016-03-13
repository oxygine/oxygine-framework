#pragma once
#include "oxygine_include.h"
#include "math/Rect.h"
#include "core/NativeTexture.h"
#include "core/Renderer.h"
#include "Material.h"
#include "Tween.h"

namespace oxygine
{
    class Actor;
    class ShaderProgram;
    class PostProcessOptions;


    class PostProcessOptions
    {
    public:
        enum
        {
            flag_singleR2T = 1,
            flag_fullscreen = 1 << 1,
        };

        PostProcessOptions(int flags = 0) : _flags(flags), _downscale(1) {}
        PostProcessOptions& fullscreen(bool enable = true) { _flags = enable ? (_flags | flag_fullscreen) : (_flags  & (~flag_fullscreen)); return *this; }
        PostProcessOptions& singleRender(bool enable = true) { _flags = enable ? (_flags | flag_singleR2T) : (_flags  & (~flag_singleR2T)); return *this; }
        //loops -(2, 3, 4, ...),  final size: 2^loops
        PostProcessOptions& downscale(int loops = 2) { _downscale = loops; return *this; }

        int _flags;
        int _downscale;
    };


    class PostProcess
    {
    public:

        static ShaderProgram* shaderBlurV;
        static ShaderProgram* shaderBlurH;
        static ShaderProgram* shaderBlit;

        static void initShaders();
        static void freeShaders();

        PostProcess(const PostProcessOptions& opt);
        ~PostProcess();

        void free();

        void update(Actor* actor);
        Rect getScreenRect(const Actor& actor) const;

        Point _extend;
        spNativeTexture _rt;
        TextureFormat _format;
        Transform _transform;
        Rect _screen;

        PostProcessOptions _options;
    };


    DECLARE_SMART(TweenPostProcess, spTweenPostProcess);

    class TweenPostProcess : public TweenObj, public Material
    {
    public:
        TweenPostProcess(const PostProcessOptions& opt);
        ~TweenPostProcess();

        Actor* getActor() const { return _actor; }

        void renderPP();

    protected:
        void init(Actor& actor) OVERRIDE;
        void update(Actor& actor, float p, const UpdateState& us) OVERRIDE;
        void done(Actor& actor) OVERRIDE;
        virtual void _renderPP() {}


        Actor* _actor;
        PostProcess _pp;
        float _progress;

        Material* _prevMaterial;
    };


    class RenderTargetsManager
    {
    public:
        RenderTargetsManager();

        spNativeTexture get(spNativeTexture current, int w, int h, TextureFormat tf);
        void update();
        void reset();

    protected:
        void print();
        bool isGood(const spNativeTexture& t, int w, int h, TextureFormat tf) const;

        typedef std::vector<spNativeTexture> rts;
        rts _rts;

        typedef std::vector<spNativeTexture> free;
        free _free;
    };

    void pass(spNativeTexture srcTexture, const Rect& srcRect, spNativeTexture destTexture, const Rect& destRect, const Color& color = Color::White);

    RenderTargetsManager& getRTManager();

    void updatePortProcessItems();
    void clearPostProcessItems();
}