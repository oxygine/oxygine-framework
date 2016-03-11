#include "TweenAlphaFade.h"
#include "core/file.h"
#include "core/gl/ShaderProgramGL.h"
#include "core/gl/oxgl.h"
#include "core/gl/VertexDeclarationGL.h"
#include "core/gl/NativeTextureGLES.h"

namespace oxygine
{

    TweenPostProcess::TweenPostProcess(int opt) : _actor(0), _prev(0), _options(opt)
    {
    }

    TweenPostProcess::~TweenPostProcess()
    {
        if (_actor)
            _actor->setMaterial(0);
        if (_rt)
            _rt->release();
        _rt = 0;
    }


    Rect TweenPostProcess::getScreenRect(const Actor& actor) const
    {
        Rect screen;

        Rect display(Point(0, 0), core::getDisplaySize());

        if (_options & opt_fullscreen)
            return display;

        screen = actor.computeBounds(actor.computeGlobalTransform()).cast<Rect>();
        screen.size += Point(1, 1);
        Point ext(25, 25);
        screen.expand(ext, ext);
        screen.clip(display);

        return screen.cast<Rect>();
    }

    void TweenPostProcess::init(Actor& actor)
    {
        _actor = &actor;
        _prev = _actor->getMaterial();
        actor.setMaterial(this);

        restore(0, 0);
    }

    void TweenPostProcess::restore(Restorable* r, void* userData)
    {
        render2texture();
    }

    void TweenPostProcess::done(Actor& actor)
    {
        actor.setMaterial(0);
        if (_rt)
            _rt->release();
        _rt = 0;
        _actor = 0;
    }

    bool TweenPostProcess::begin()
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

    void TweenPostProcess::rtCreated()
    {
        _rt->reg(CLOSURE(this, &TweenPostProcess::restore), 0);
    }

    void TweenPostProcess::render2texture()
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
        _rtTransform = _actor->computeGlobalTransform().inverted();

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

    void TweenPostProcess::update(Actor& actor, float p, const UpdateState& us)
    {
        if (!(_options & opt_singleR2T))
            render2texture();
    }

    void TweenPostProcess::apply(Material* prev)
    {

    }


    TweenAlphaFade::TweenAlphaFade(bool fadeIn, int opt) : TweenPostProcess(opt), _fadeIn(fadeIn), _a(0)
    {
    }

    void TweenAlphaFade::update(Actor& actor, float p, const UpdateState& us)
    {
        TweenPostProcess::update(actor, p, us);
        _a = lerp(_fadeIn ? 0 : 255, _fadeIn ? 255 : 0, p);
    }

    void TweenAlphaFade::render(Actor* actor, const RenderState& rs)
    {
        STDMaterial* mat = STDMaterial::instance;
        STDRenderer* renderer = mat->getRenderer();

        RectF src(0, 0,
                  _screen.getWidth() / (float)_rt->getWidth(),
                  _screen.getHeight() / (float)_rt->getHeight());
        RectF dest = _screen.cast<RectF>();

        renderer->setBlendMode(blend_premultiplied_alpha);
        AffineTransform tr = _rtTransform * _actor->computeGlobalTransform();

        renderer->setTransform(tr);
        renderer->beginElementRendering(true);
        Color color = Color(Color::White).withAlpha(_a).premultiplied();
        renderer->drawElement(_rt, color.rgba(), src, dest);
        renderer->drawBatch();
    }



    ShaderProgram* TweenGlow::shaderBlurV = 0;
    ShaderProgram* TweenGlow::shaderBlurH = 0;
    ShaderProgram* TweenGlow::shaderBlit = 0;


    void TweenGlow::render2texture()
    {
        TweenPostProcess::render2texture();
        const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2::FORMAT));

        if (!shaderBlurH)
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


            shaderBlurV = new ShaderProgramGL(ShaderProgramGL::createProgram(v, ps, decl));
            shaderBlurH = new ShaderProgramGL(ShaderProgramGL::createProgram(h, ps, decl));



            file::buffer vs_blit;
            file::buffer fs_blit;
            file::read("pp_blit_vs.glsl", vs_blit);
            file::read("pp_blit_fs.glsl", fs_blit);

            vs_blit.push_back(0);
            fs_blit.push_back(0);


            unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_blit.front(), "", "");
            unsigned int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blit.front(), "", "");

            shaderBlit = new ShaderProgramGL(ShaderProgramGL::createProgram(vs, fs, decl));
        }


        GLint prevFBO = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

        Rect vp = _screen;
        vp.pos = Point(0, 0);

        vertexPCT2 v[4];



        int w = _screen.size.x;
        int h = _screen.size.y;

        IVideoDriver* driver = IVideoDriver::instance;

        driver->setState(IVideoDriver::STATE_BLEND, 0);
        /*

        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt2.get())->getFboID());
        driver->setViewport(Rect(0, 0, w / 2, h / 2));
        driver->setShaderProgram(shaderBlit);
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt);


        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt->getWidth(), h / (float)_rt->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);
        w /= 2;
        h /= 2;


        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt.get())->getFboID());
        driver->setViewport(Rect(0, 0, w / 2, h / 2));
        driver->setShaderProgram(shaderBlit);
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt2);


        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt2->getWidth(), h / (float)_rt2->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);
        w /= 2;
        h /= 2;

        */


        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt2.get())->getFboID());

        driver->setViewport(Rect(0, 0, w, h));
        driver->clear(0);
        driver->setShaderProgram(shaderBlurH);
        driver->setUniform("step", 1.0f / _rt->getWidth());
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt);



        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt->getWidth(), h / (float)_rt->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);



        oxglBindFramebuffer(GL_FRAMEBUFFER, safeCast<NativeTextureGLES*>(_rt.get())->getFboID());


        driver->setViewport(Rect(0, 0, w, h));
        driver->clear(Color::Red);

        driver->setShaderProgram(shaderBlurV);
        driver->setUniform("step", 1.0f / _rt2->getHeight());
        driver->setUniformInt("s_texture", 0);
        driver->setTexture(0, _rt2);

        fillQuadT(v,
                  RectF(0, 0, w / (float)_rt2->getWidth(), h / (float)_rt2->getHeight()),
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), 0);


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);




        oxglBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
    }

    void TweenGlow::rtCreated()
    {
        TweenPostProcess::rtCreated();
        _rt2 = IVideoDriver::instance->createTexture();
        //_rt2->init(_screen.getWidth() / 2, _screen.getHeight() / 2, TF_R8G8B8A8, true);
        _rt2->init(_screen.getWidth(), _screen.getHeight(), TF_R8G8B8A8, true);
    }
}