#include "TweenAlphaFade.h"
#include "core/file.h"
#include "core/gl/ShaderProgramGL.h"
#include "core/gl/oxgl.h"
#include "core/gl/VertexDeclarationGL.h"
#include "core/gl/NativeTextureGLES.h"
#include <vector>

namespace oxygine
{
    using namespace std;

    DECLARE_SMART(TweenPostProcess, spTweenPostProcess);

    vector<spTweenPostProcess> postProcessItems;

    int next256(int v)
    {
        const int M = 64;//256

        int n = (v - 1) / M;
        return (n + 1) * M;
    }

    class NTP
    {
    public:
        int _w;
        int _h;
        TextureFormat _tf;
        NTP(int w, int h, TextureFormat tf): _w(w), _h(h), _tf(tf) {}

        bool operator()(const spNativeTexture& t1, const spNativeTexture& t2) const
        {
            if (t1->getFormat() > _tf)
                return true;
            if (t1->getWidth() > _w)
                return true;
            return t1->getHeight() > _h;
        }

        static bool comparePred2(const spNativeTexture& t2, const spNativeTexture& t1)
        {
            if (t1->getFormat() > t2->getFormat())
                return true;
            if (t1->getWidth() > t2->getWidth())
                return true;
            return t1->getHeight() > t2->getHeight();
        }
    };

    class RTManager
    {
    public:
        RTManager(): _started(false) {}

        spNativeTexture get(int w, int h, TextureFormat tf)
        {
            w = next256(w);
            h = next256(h);

            spNativeTexture texture;
            free::iterator it = lower_bound(_free.begin(), _free.end(), texture, NTP(w, h, tf));
            for (size_t i = 0; i < _free.size(); ++i)
            {
                spNativeTexture t = _free[i];
                if (!(t->getWidth() >= w && t->getHeight() >= h && t->getFormat() == tf))
                    continue;

                texture = t;
                _free.erase(_free.begin() + i);
                break;
            }

            if (!texture)
            {
                texture = IVideoDriver::instance->createTexture();
                texture->init(w, h, tf, true);
            }

            rt r;
            r.texture = texture;
            r.tm = getTimeMS();
            _rts.push_back(r);


            if (_started)
            {
                _started = true;
                //core::getMainThreadMessages().postCallback()
            }

            return texture;
        }



        void update()
        {
            timeMS tm = getTimeMS();
            for (size_t i = 0; i < _rts.size(); ++i)
            {
                rt& r = _rts[i];
                if (r.texture->_ref_counter == 1)
                {
                    free::iterator it = lower_bound(_free.begin(), _free.end(), r.texture, NTP::comparePred2);
                    _free.insert(it, r.texture);
                    _rts.erase(_rts.begin() + i);
                    i--;
                    continue;
                }
                if (r.tm + 3000 < tm)
                {
                    //r.texture->release();
                }

            }
        }

        spNativeTexture abc(spNativeTexture t)
        {
            return t;
            //if (t->getHandle() == 0)
            //return
            for (size_t i = 0; i < _rts.size(); ++i)
            {
                rt& r = _rts[i];
                if (r.texture == t)
                {
                    _free.push_back(r.texture);
                    _rts.erase(_rts.begin() + i);
                    i--;
                    break;
                }
            }

            return t;
        }

    protected:
        struct rt
        {
            rt(): free(true), tm(0) {}
            bool free;
            spNativeTexture texture;
            timeMS tm;
        };

        typedef std::vector<rt> rts;
        rts _rts;

        typedef std::vector<spNativeTexture> free;
        free _free;

        bool _started;
    };

    RTManager rtm;
    RTManager& getRTManager()
    {
        return rtm;
    }

    class PostProcess
    {
    public:

        static ShaderProgram* shaderBlurV;
        static ShaderProgram* shaderBlurH;
        static ShaderProgram* shaderBlit;

        PostProcess(const PostProcessOptions& opt);
        ~PostProcess();

        void free();

        void update(Actor* actor);
        Rect getScreenRect(const Actor& actor) const;

        Point _extend;
        spNativeTexture _rt;
        TextureFormat _format;
        Transform _rtTransform;
        Rect _screen;

        PostProcessOptions _options;
    };




    ShaderProgram* PostProcess::shaderBlurV = 0;
    ShaderProgram* PostProcess::shaderBlurH = 0;
    ShaderProgram* PostProcess::shaderBlit = 0;



    DECLARE_SMART(TweenPostProcess, spTweenPostProcess);

    class TweenPostProcess : public TweenObj, public Material
    {
    public:
        TweenPostProcess(const PostProcessOptions& opt);
        ~TweenPostProcess();


        void renderPP();


        Actor* _actor;

    protected:
        void init(Actor& actor) OVERRIDE;
        void update(Actor& actor, float p, const UpdateState& us) OVERRIDE;
        void done(Actor& actor) OVERRIDE;
        virtual void _renderPP() {}

        PostProcess _pp;
        float _progress;

        Material* _prevMaterial;
    };

    void renderPostProcessItems()
    {
        if (!postProcessItems.empty())
        {
            IVideoDriver* driver = IVideoDriver::instance;
            spNativeTexture prevRT = driver->getRenderTarget();

            for (size_t i = 0; i < postProcessItems.size(); ++i)
            {
                spTweenPostProcess p = postProcessItems[i];
                p->renderPP();
                p->_actor->releaseRef();
            }

            postProcessItems.clear();
            driver->setRenderTarget(prevRT);
        }
        getRTManager().update();
    }


    void pass(spNativeTexture srcTexture, const Rect& srcRect, spNativeTexture destTexture, const Rect& destRect, const Color& color = Color::White)
    {
        IVideoDriver* driver = IVideoDriver::instance;

        const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(driver->getVertexDeclaration(vertexPCT2::FORMAT));
        driver->setRenderTarget(destTexture);
        driver->clear(0);

        driver->setViewport(destRect);

        driver->setTexture(0, srcTexture);


        vertexPCT2 v[4];


        RectF dst = srcRect.cast<RectF>() / Vector2(srcTexture->getWidth(), srcTexture->getHeight());
        fillQuadT(v,
                  dst,
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), color.rgba());


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);
    }

    PostProcess::PostProcess(const PostProcessOptions& opt): _options(opt), _format(TF_R4G4B4A4), _extend(2, 2)
    {
    }

    PostProcess::~PostProcess()
    {
        //if (_rt)
        //   _rt->release();
    }

    void PostProcess::free()
    {
        //if (_rt)
        //    _rt->release();
        _rt = 0;
    }

    Rect PostProcess::getScreenRect(const Actor& actor) const
    {
        Rect screen;

        Rect display(Point(0, 0), core::getDisplaySize());

        if (_options._flags & PostProcessOptions::fullscreen)
            return display;

        screen = actor.computeBounds(actor.computeGlobalTransform()).cast<Rect>();
        screen.size += Point(1, 1);
        screen.expand(_extend, _extend);

        if (!(_options._flags & PostProcessOptions::singleR2T))
            screen.clip(display);

        return screen.cast<Rect>();
    }

    void PostProcess::update(Actor* actor)
    {
        _screen = getScreenRect(*actor);

        OX_ASSERT(actor->_getStage());
        _rt = getRTManager().get(_screen.getWidth(), _screen.getHeight(), _format);


        _rtTransform = actor->computeGlobalTransform().inverted();




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

        rs.transform = actor->getParent()->computeGlobalTransform();


        if (!(_options._flags & PostProcessOptions::fullscreen))
        {
            AffineTransform offset;
            offset.identity();
            offset.translate(-_screen.pos);
            rs.transform = rs.transform * offset;
        }

        mat->Material::render(actor, rs);

        mat->finish();
    }





    TweenPostProcess::TweenPostProcess(const PostProcessOptions& opt) : _pp(opt), _prevMaterial(0), _actor(0)
    {
    }

    TweenPostProcess::~TweenPostProcess()
    {
        if (_actor)
            _actor->setMaterial(_prevMaterial);
    }


    void TweenPostProcess::renderPP()
    {
        if (_pp._options._flags & PostProcessOptions::singleR2T)
            return;


        _pp.update(_actor);
        _renderPP();
    }

    void TweenPostProcess::init(Actor& actor)
    {
        _actor = &actor;
        _prevMaterial = _actor->getMaterial();
        _actor->setMaterial(this);
    }

    void TweenPostProcess::update(Actor& actor, float p, const UpdateState& us)
    {
        //_pp.update(_actor);
        _progress = p;

        if (find(postProcessItems.begin(), postProcessItems.end(), this) == postProcessItems.end())
        {
            _actor->addRef();
            postProcessItems.push_back(this);
        }
    }

    void TweenPostProcess::done(Actor& actor)
    {
        _actor->setMaterial(_prevMaterial);
    }




    class TweenOutlineImpl : public TweenPostProcess
    {
    public:
        Color _color;
        int _downsample;

        TweenOutlineImpl(const Color& c, const PostProcessOptions& opt) : TweenPostProcess(opt), _color(c), _downsample(1) {}

        void render(Actor* actor, const RenderState& rs) OVERRIDE
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
            actor->setMaterial(_prevMaterial);
            actor->render(r);
            actor->setMaterial(this);
        }

        void _renderPP() OVERRIDE
        {
            IVideoDriver* driver = IVideoDriver::instance;

            const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2::FORMAT));

            if (!PostProcess::shaderBlurH)
            {
                file::buffer vs_h;
                file::buffer vs_v;
                file::buffer fs_blur;
                file::read("pp_hblur_vs.glsl", vs_h);
                file::read("pp_vblur_vs.glsl", vs_v);
                file::read("pp_rast_fs.glsl", fs_blur);

                vs_h.push_back(0);
                vs_v.push_back(0);
                fs_blur.push_back(0);


                unsigned int h = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_h.front(), "", "");
                unsigned int v = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_v.front(), "", "");
                unsigned int ps = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blur.front(), "", "");


                PostProcess::shaderBlurV = new ShaderProgramGL(ShaderProgramGL::createProgram(v, ps, decl));
                driver->setShaderProgram(PostProcess::shaderBlurV);
                driver->setUniformInt("s_texture", 0);

                PostProcess::shaderBlurH = new ShaderProgramGL(ShaderProgramGL::createProgram(h, ps, decl));
                driver->setShaderProgram(PostProcess::shaderBlurH);
                driver->setUniformInt("s_texture", 0);


                file::buffer vs_blit;
                file::buffer fs_blit;
                file::read("pp_blit_vs.glsl", vs_blit);
                file::read("pp_blit_fs.glsl", fs_blit);

                vs_blit.push_back(0);
                fs_blit.push_back(0);


                unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_blit.front(), "", "");
                unsigned int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blit.front(), "", "");

                PostProcess::shaderBlit = new ShaderProgramGL(ShaderProgramGL::createProgram(vs, fs, decl));
                driver->setShaderProgram(PostProcess::shaderBlit);
                driver->setUniformInt("s_texture", 0);
            }


            int w = _pp._screen.size.x;
            int h = _pp._screen.size.y;


            driver->setState(IVideoDriver::STATE_BLEND, 0);

            _downsample = 1;


            spNativeTexture rt = _pp._rt;
            spNativeTexture rt2 = getRTManager().get(w, h, _pp._format);

#if 1
            driver->setShaderProgram(PostProcess::shaderBlit);
            pass(rt, Rect(0, 0, w, h), rt2, Rect(0, 0, w / 2, h / 2));

            w /= 2;
            h /= 2;
            _downsample *= 2;
#endif

#if 1

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


            int alpha = lerp(255, 0, _progress);

            Color c = _color.withAlpha(alpha).premultiplied();

            driver->setShaderProgram(PostProcess::shaderBlurV);
            driver->setUniform("step", 1.0f / rt2->getHeight());

            pass(rt2, rc, rt, rc, c);
        }
    };


    class TweenAlphaFadeImpl : public TweenPostProcess
    {
    public:
        TweenAlphaFadeImpl(bool fadeIn, const PostProcessOptions& opt) : TweenPostProcess(opt), _fadeIn(fadeIn) {}

        void render(Actor* actor, const RenderState& rs)
        {
            int _a = lerp(_fadeIn ? 0 : 255, _fadeIn ? 255 : 0, _progress);
            STDMaterial* mat = STDMaterial::instance;
            STDRenderer* renderer = mat->getRenderer();

            spNativeTexture rt = _pp._rt;
            RectF src(0, 0,
                      _pp._screen.getWidth() / (float)rt->getWidth(),
                      _pp._screen.getHeight() / (float)rt->getHeight());
            RectF dest = _pp._screen.cast<RectF>();

            renderer->setBlendMode(blend_premultiplied_alpha);
            AffineTransform tr = _pp._rtTransform * _actor->computeGlobalTransform();

            renderer->setTransform(tr);
            renderer->beginElementRendering(true);
            Color color = Color(Color::White).withAlpha(_a).premultiplied();
            renderer->drawElement(rt, color.rgba(), src, dest);
            renderer->drawBatch();
        }

        bool _fadeIn;
    };











    TweenAlphaFade::TweenAlphaFade(bool fadeIn, const PostProcessOptions& opt): TweenProxy(new TweenAlphaFadeImpl(fadeIn, opt))
    {
    }

    TweenOutline::TweenOutline(const Color& color, const PostProcessOptions& opt) : TweenProxy(new TweenOutlineImpl(color, opt))
    {
    }
}