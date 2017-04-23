#include "PostProcess.h"
#include "Actor.h"
#include "core/gl/VertexDeclarationGL.h"
#include "core/gl/ShaderProgramGL.h"
#include "core/oxygine.h"
#include "RenderState.h"
#include "STDMaterial.h"
#include "core/file.h"
#include "core/system_data.h"
#include "core/ZipFileSystem.h"

namespace oxygine
{
    ShaderProgram* PostProcess::shaderBlurV = 0;
    ShaderProgram* PostProcess::shaderBlurH = 0;
    ShaderProgram* PostProcess::shaderBlit = 0;
    bool _ppBuilt = false;

    void PostProcess::initShaders()
    {
        if (_ppBuilt)
            return;
        _ppBuilt = true;



        IVideoDriver* driver = IVideoDriver::instance;



        file::Zips zp;
        zp.add(system_data, system_size);


        const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(IVideoDriver::instance->getVertexDeclaration(vertexPCT2::FORMAT));

        file::buffer vs_h;
        file::buffer vs_v;
        file::buffer fs_blur;
        zp.read("system/pp_hblur_vs.glsl", vs_h);
        zp.read("system/pp_vblur_vs.glsl", vs_v);
        zp.read("system/pp_rast_fs.glsl", fs_blur);

        vs_h.push_back(0);
        vs_v.push_back(0);
        fs_blur.push_back(0);


        unsigned int h = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_h.front());
        unsigned int v = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_v.front());
        unsigned int ps = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blur.front());


        shaderBlurV = new ShaderProgramGL(ShaderProgramGL::createProgram(v, ps, decl));
        driver->setShaderProgram(shaderBlurV);
        driver->setUniformInt("s_texture", 0);

        shaderBlurH = new ShaderProgramGL(ShaderProgramGL::createProgram(h, ps, decl));
        driver->setShaderProgram(shaderBlurH);
        driver->setUniformInt("s_texture", 0);


        oxglDeleteShader(h);
        oxglDeleteShader(v);
        oxglDeleteShader(ps);

        file::buffer vs_blit;
        file::buffer fs_blit;
        zp.read("system/pp_blit_vs.glsl", vs_blit);
        zp.read("system/pp_blit_fs.glsl", fs_blit);

        vs_blit.push_back(0);
        fs_blit.push_back(0);


        unsigned int vs = ShaderProgramGL::createShader(GL_VERTEX_SHADER, (const char*)&vs_blit.front(), "", "");
        unsigned int fs = ShaderProgramGL::createShader(GL_FRAGMENT_SHADER, (const char*)&fs_blit.front(), "", "");

        shaderBlit = new ShaderProgramGL(ShaderProgramGL::createProgram(vs, fs, decl, true));
        driver->setShaderProgram(shaderBlit);
        driver->setUniformInt("s_texture", 0);
    }

    void PostProcess::freeShaders()
    {
        delete shaderBlit;
        shaderBlit = 0;

        delete shaderBlurH;
        shaderBlurH = 0;

        delete shaderBlurV;
        shaderBlurV = 0;
    }

    const int ALIGN_SIZE = 256;
    const int TEXTURE_LIVE = 3000;
    const int MAX_FREE_TEXTURES = 3;

    using namespace std;

    DECLARE_SMART(TweenPostProcess, spTweenPostProcess);

    class PPTask;
    vector<PPTask*> postProcessItems;

    int alignTextureSize(int v)
    {
        int n = (v - 1) / ALIGN_SIZE;
        return (n + 1) * ALIGN_SIZE;
    }

    class NTP
    {
    public:
        int _w;
        int _h;
        TextureFormat _tf;
        NTP(int w, int h, TextureFormat tf) : _w(w), _h(h), _tf(tf) {}

        bool operator()(const spNativeTexture& t1, const spNativeTexture& t2) const
        {
            if (t1->getFormat() < _tf)
                return true;
            if (t1->getWidth() < _w)
                return true;
            return t1->getHeight() < _h;
        }

        static bool cmp(const spNativeTexture& t2, const spNativeTexture& t1)
        {
            if (t1->getFormat() > t2->getFormat())
                return true;
            if (t1->getWidth() > t2->getWidth())
                return true;
            return t1->getHeight() > t2->getHeight();
        }
    };


    RenderTargetsManager::RenderTargetsManager()
    {
        //get(10, 15, TF_R8G8B8A8);
        //get(10, 15, TF_R8G8B8A8);
    }

    void RenderTargetsManager::print()
    {
        log::messageln("print");
        for (size_t i = 0, sz = _free.size(); i < sz; ++i)
        {
            spNativeTexture t = _free[i];
            log::messageln("texture %d %d", t->getWidth(), t->getHeight());
        }
    }
    bool RenderTargetsManager::isGood(const spNativeTexture& t, int w, int h, TextureFormat tf) const
    {
        if (!t)
            return false;

        if (!t->getHandle())
            return false;

        if (!HAVE_NPOT_RT())
        {
            w = nextPOT(w);
            h = nextPOT(h);
        }

        if (t->getFormat() == tf &&
                t->getWidth() >= w && t->getHeight() >= h &&
                t->getWidth() <= (w + ALIGN_SIZE) && t->getHeight() <= (h + ALIGN_SIZE))
            return true;
        return false;
    }

    spNativeTexture RenderTargetsManager::get(spNativeTexture current, int w, int h, TextureFormat tf)
    {
        w = alignTextureSize(w);
        h = alignTextureSize(h);
        if (isGood(current, w, h, tf))
        {
            current->setUserData((void*)(size_t)getTimeMS());
            return current;
        }


        spNativeTexture result;

        free::iterator it = lower_bound(_free.begin(), _free.end(), result, NTP(w, h, tf));
        if (it != _free.end())
        {
            spNativeTexture& t = *it;
            if (isGood(t, w, h, tf))
            {
                result = t;
                _free.erase(it);
            }
        }

        if (!result)
        {
            //if texture wasn't found create it
            result = IVideoDriver::instance->createTexture();
            result->init(w, h, tf, true);
        }

        result->setUserData((void*)(size_t)getTimeMS());
        _rts.push_back(result);

        //print();

        return result;
    }



    void RenderTargetsManager::update()
    {
        timeMS tm = getTimeMS();
        for (size_t i = 0, sz = _rts.size(); i < sz; ++i)
        {
            spNativeTexture& texture = _rts[i];
            if (texture->_ref_counter == 1)
            {
                free::iterator it = lower_bound(_free.begin(), _free.end(), texture, NTP::cmp);
                _free.insert(it, texture);
                _rts.erase(_rts.begin() + i);
                --i;
                --sz;
                continue;
            }
        }

        for (size_t i = 0, sz = _free.size(); i < sz; ++i)
        {
            spNativeTexture& t = _free[i];
            timeMS createTime = (timeMS)(size_t)t->getUserData();
            if (createTime + TEXTURE_LIVE > tm)
                continue;
            _free.erase(_free.begin() + i);
            --i;
            --sz;
        }

        if (_free.size() > MAX_FREE_TEXTURES)
        {
            _free.erase(_free.begin(), _free.begin() + _free.size() - MAX_FREE_TEXTURES);
        }
    }

    void RenderTargetsManager::reset()
    {
        for (size_t i = 0; i < _rts.size(); ++i)
        {
            _rts[i]->release();
        }

        _free.clear();
        _rts.clear();
    }

    RenderTargetsManager _rtm;
    RenderTargetsManager& getRTManager()
    {
        return _rtm;
    }


    void addPostProcessItem(PPTask* task)
    {
        if (find(postProcessItems.begin(), postProcessItems.end(), task) == postProcessItems.end())
        {
            task->addRefPP();
            postProcessItems.push_back(task);
        }
    }


    void removePostProcessItem(PPTask* t)
    {
        vector<PPTask*>::iterator i = std::find(postProcessItems.begin(), postProcessItems.end(), t);
        if (i == postProcessItems.end())
            return;
        t->releaseRefPP();
        postProcessItems.erase(i);
    }


    bool _renderingPP = false;
    bool isRenderingPostProcessItems()
    {
        return _renderingPP;
    }

    void updatePortProcessItems()
    {
        if (!postProcessItems.empty())
        {
            _renderingPP = true;

            IVideoDriver* driver = IVideoDriver::instance;
            //driver->setState(IVideoDriver::STATE_BLEND, 0);
            spNativeTexture prevRT = driver->getRenderTarget();


            ShaderProgram* sp = driver->getShaderProgram();

            for (size_t i = 0; i < postProcessItems.size(); ++i)
            {
                PPTask* p = postProcessItems[i];
                p->renderPP();
                p->releaseRefPP();
            }

            postProcessItems.clear();
            driver->setRenderTarget(prevRT);
            driver->setShaderProgram(sp);
            _renderingPP = false;
        }

        _rtm.update();
    }

    void clearPostProcessItems()
    {
        postProcessItems.clear();
        _rtm.reset();
    }


    void pass(spNativeTexture srcTexture, const Rect& srcRect, spNativeTexture destTexture, const Rect& destRect, const Color& color)
    {
        IVideoDriver* driver = IVideoDriver::instance;

        const VertexDeclarationGL* decl = static_cast<const VertexDeclarationGL*>(driver->getVertexDeclaration(vertexPCT2::FORMAT));
        driver->setRenderTarget(destTexture);
        driver->clear(0);

        driver->setViewport(destRect);

        driver->setTexture(0, srcTexture);


        vertexPCT2 v[4];


        RectF dst = srcRect.cast<RectF>() / Vector2((float)srcTexture->getWidth(), (float)srcTexture->getHeight());
        fillQuadT(v,
                  dst,
                  RectF(-1, -1, 2, 2),
                  AffineTransform::getIdentity(), color.rgba());


        driver->draw(IVideoDriver::PT_TRIANGLE_STRIP, decl, v, sizeof(v));
        driver->setTexture(0, 0);
    }

    PostProcess::PostProcess(const PostProcessOptions& opt) : _options(opt), _format(TF_R4G4B4A4), _extend(2, 2)
    {
    }

    PostProcess::~PostProcess()
    {
    }

    void PostProcess::free()
    {
        _rt = 0;
    }

    Rect PostProcess::getScreenRect(const Actor& actor) const
    {
        if (_options._flags & PostProcessOptions::flag_screen)
            return _screen;

        Rect screen;

        Rect display(Point(0, 0), core::getDisplaySize());

        if (_options._flags & PostProcessOptions::flag_fullscreen)
            return display;

        screen = actor.computeBounds(actor.computeGlobalTransform()).cast<Rect>();
        screen.size += Point(1, 1);
        screen.expand(_extend, _extend);

        if (!(_options._flags & PostProcessOptions::flag_singleR2T))
            screen.clip(display);

        return screen.cast<Rect>();
    }

    void PostProcess::update(Actor* actor)
    {
        _screen = getScreenRect(*actor);
        if (_screen.isEmpty())
            return;

        _rt = getRTManager().get(_rt, _screen.getWidth(), _screen.getHeight(), _format);


        _transform = actor->computeGlobalTransform().inverted();


        MaterialX::null->apply();


        IVideoDriver* driver = IVideoDriver::instance;

        driver->setRenderTarget(_rt);

        Rect vp = _screen;
        vp.pos = Point(0, 0);
        driver->setViewport(vp);

        driver->clear(_options._clearColor);


        RenderState rs;
        STDRenderer* renderer = STDRenderer::instance;


        RectF clip = vp.cast<RectF>();
        rs.clip = &clip;

        renderer->initCoordinateSystem(vp.getWidth(), vp.getHeight(), true);

        rs.transform = actor->getParent()->computeGlobalTransform();


        if (!(_options._flags & PostProcessOptions::flag_fullscreen))
        {
            AffineTransform offset;
            offset.identity();
            offset.translate(-_screen.pos);
            rs.transform = rs.transform * offset;
        }

        //OX_ASSERT(0);
        Material* rd = actor->getMaterial();
        actor->setMaterial(STDMaterial::instance);
        STDMaterial::instance->Material::render(actor, rs);

        STDRenderer::current->flush();

        actor->setMaterial(rd);

        MaterialX::null->apply();
    }





    TweenPostProcess::TweenPostProcess(const PostProcessOptions& opt) : _pp(opt), _prevMaterial(0), _actor(0)
    {
    }

    TweenPostProcess::~TweenPostProcess()
    {

        removePostProcessItem(this);
        if (_actor && _actor->getMaterial())
            _actor->setMaterial(_prevMaterial);
    }


    void TweenPostProcess::renderPP()
    {
        if (_pp._options._flags & PostProcessOptions::flag_singleR2T && _pp._rt)
            return;

        _pp.update(_actor);
        _renderPP();
    }

    void TweenPostProcess::addRefPP()
    {
        _actor->addRef();
    }

    void TweenPostProcess::releaseRefPP()
    {
        _actor->releaseRef();
    }

    void TweenPostProcess::init(Actor& actor)
    {
        _actor = &actor;
        _prevMaterial = _actor->getMaterial();
        _actor->setMaterial(this);
    }

    void TweenPostProcess::update(Actor& actor, float p, const UpdateState& us)
    {
        _progress = p;
        addPostProcessItem(this);
    }

    void TweenPostProcess::done(Actor& actor)
    {
        if (_actor->getMaterial())
            _actor->setMaterial(_prevMaterial);
    }

}