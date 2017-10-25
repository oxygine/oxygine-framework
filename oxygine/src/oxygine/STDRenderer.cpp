#include "STDRenderer.h"
#include "core/UberShaderProgram.h"
#include "core/VertexDeclaration.h"
#include "STDMaterial.h"
#include "Image.h"
#include "core/file.h"
#include "core/ZipFileSystem.h"
#include "core/system_data.h"
#include "math/Rect.h"
#include "Image.h"
#include "core/ImageDataOperations.h"
#include "AnimationFrame.h"
#include "core/VertexDeclaration.h"
#include "core/gl/VideoDriverGLES20.h"

#include "core/UberShaderProgram.h"

#include "RenderState.h"
#include "VisualStyle.h"

//#define EXP_SORT

namespace oxygine
{
    bool _restored = false;
    STDRenderer* STDRenderer::instance = 0;
    STDRenderer* STDRenderer::current = 0;


    spNativeTexture STDRenderer::white;
    spNativeTexture STDRenderer::invisible;

    std::vector<unsigned short> STDRenderer::indices16;
    size_t STDRenderer::maxVertices = 0;
    UberShaderProgram STDRenderer::uberShader;
    std::vector<unsigned char> STDRenderer::uberShaderBody;


    RenderStateCache& rsCache()
    {
        static RenderStateCache r(IVideoDriver::instance);
        return r;
    }

    RenderStateCache::RenderStateCache(IVideoDriver* d) : _driver(d), _blend(blend_disabled), _program(0)
    {
        reset();
    }

    void RenderStateCache::reset()
    {
        resetTextures();

        _blend = blend_disabled;
        _driver->setState(IVideoDriver::STATE_BLEND, 0);
        _program = 0;
    }

    void RenderStateCache::resetTextures()
    {
        for (int i = 0; i < MAX_TEXTURES; ++i)
            _textures[i] = 0;
    }

    void RenderStateCache::setTexture(int sampler, const spNativeTexture& t)
    {
        OX_ASSERT(sampler < MAX_TEXTURES);

#ifdef OX_DEBUG
        if (_textures[sampler] && _driver == IVideoDriver::instance)
        {
            GLint whichID;
            oxglActiveTexture(GL_TEXTURE0 + sampler);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &whichID);

            OX_ASSERT(_textures[sampler]->getHandle() == (nativeTextureHandle)whichID);
        }
#endif

        if (_textures[sampler] == t)
            return;
        _textures[sampler] = t;
        _driver->setTexture(sampler, t);
    }


    void RenderStateCache::setBlendMode(blend_mode blend)
    {
        if (_blend == blend)
            return;


        if (blend == 0)
            _driver->setState(IVideoDriver::STATE_BLEND, 0);
        else
        {
            IVideoDriver::BLEND_TYPE src = static_cast<IVideoDriver::BLEND_TYPE>(blend >> 16);
            IVideoDriver::BLEND_TYPE dest = static_cast<IVideoDriver::BLEND_TYPE>(blend & 0xFFFF);
            _driver->setBlendFunc(src, dest);
            _driver->setState(IVideoDriver::STATE_BLEND, 1);
        }
        _blend = blend;

    }

    void RenderStateCache::changeDriver(IVideoDriver* d)
    {
        _driver = d;
    }


    bool RenderStateCache::setShader(ShaderProgram* prog)
    {
        if (_program == prog)
            return false;

        _program = prog;
        _driver->setShaderProgram(prog);
        return true;
    }

    void nullTextureHook(const spNativeTexture&)
    {
    }

    render_texture_hook _renderTextureHook = nullTextureHook;


    render_texture_hook get_render_texture_hook()
    {
        return _renderTextureHook;
    }

    void set_render_texture_hook(render_texture_hook h)
    {
        _renderTextureHook = h;
    }

    template<class V, class XY>
    void fillQuad(V* v, const RectF& uv, XY* positions, const Color& color)
    {
        const XY& p1 = positions[0];
        const XY& p2 = positions[1];
        const XY& p3 = positions[2];
        const XY& p4 = positions[3];

        V vt;
        vt.color = color.rgba();
        vt.z = 0;

        vt.x = p1.x;
        vt.y = p1.y;
        vt.u = uv.pos.x;
        vt.v = uv.pos.y;
        *v = vt;
        ++v;

        vt.x = p2.x;
        vt.y = p2.y;
        vt.u = uv.pos.x;
        vt.v = uv.getBottom();
        *v = vt;
        ++v;

        vt.x = p3.x;
        vt.y = p3.y;
        vt.u = uv.getRight();
        vt.v = uv.pos.y;
        *v = vt;
        ++v;

        vt.x = p4.x;
        vt.y = p4.y;
        vt.u = uv.getRight();
        vt.v = uv.getBottom();
        *v = vt;
        ++v;
    }

    void STDRenderer::initialize()
    {
        indices16.reserve(3000 * 6);
        for (int t = 0; t < 3000; t += 1)
        {
            int i = t * 4;
            indices16.push_back(i + 0);
            indices16.push_back(i + 1);
            indices16.push_back(i + 2);

            indices16.push_back(i + 2);
            indices16.push_back(i + 1);
            indices16.push_back(i + 3);
        }

        maxVertices = indices16.size() / 3 * 2;

        file::buffer shaderBody;
        file::read("shader.glsl", shaderBody, ep_ignore_error);
        uberShaderBody = shaderBody.data;
        if (!shaderBody.getSize())
        {
            file::Zips zp;
            zp.add(system_data, system_size);
            zp.read("system/shader.glsl", shaderBody);
            uberShaderBody = shaderBody.data;
        }

        uberShader.init(uberShaderBody);

        restore();
    }

    void STDRenderer::release()
    {
        indices16.clear();
        uberShader.release();
        uberShaderBody.clear();
        if (white)
            white->release();
        white = 0;

        if (invisible)
            invisible->release();
        invisible = 0;

        delete instance;
        instance = 0;
    }

    void STDRenderer::reset()
    {
        _restored = false;
        if (white)
            white->release();
        white = 0;

        if (invisible)
            invisible->release();
        invisible = 0;

        uberShader.release();
    }

    bool STDRenderer::isReady()
    {
        return _restored;
    }

    void STDRenderer::restore()
    {
        Image memwhite;
        memwhite.init(4, 4, TF_R8G8B8A8);

        oxygine::operations::op_fill fill;
        ImageData im = memwhite.lock();
        oxygine::operations::applyOperation(fill, im);

        white = IVideoDriver::instance->createTexture();
        white->setName("!renderer. white");
        white->init(im, false);
        white->setLinearFilter(false);


        memwhite.fillZero();
        invisible = IVideoDriver::instance->createTexture();
        invisible->setName("!renderer. invisible");
        invisible->init(im, false);
        invisible->setLinearFilter(false);

        _restored = true;
    }


    STDRenderer* STDRenderer::getCurrent()
    {
        return current;
    }

    STDRenderer::~STDRenderer()
    {
        flush();
    }

    const oxygine::Matrix& STDRenderer::getViewProjection() const
    {
        return _vp;
    }

    void STDRenderer::setShader(ShaderProgram* prog)
    {
        if (rsCache().setShader(prog))
        {
            //_driver->setUniform("mat", _vp);
            shaderProgramChanged();

            ShaderProgramChangedHook* hook = _sphookFirst;
            while (hook)
            {
                hook->hook();
                hook = hook->next;
            }
        }
    }


    void STDRenderer::xdrawBatch()
    {
        size_t count = _verticesData.size() / _vdecl->size;
        size_t indices = (count * 3) / 2;

        getDriver()->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_verticesData.front(), (unsigned int)count, &indices16.front(), (unsigned int)indices);

        _verticesData.clear();
    }

    void STDRenderer::initCoordinateSystem(int width, int height, bool flipU)
    {
        Matrix view = makeViewMatrix(width, height, flipU);
        Matrix proj;
        //initialize projection matrix
        Matrix::orthoLH(proj, (float)width, (float)height, 0, 1);

        Matrix vp = view * proj;
        setViewProj(vp);
    }

    IVideoDriver* STDRenderer::getDriver()
    {
        return _driver;
    }


    void STDRenderer::setViewProj(const Matrix& viewProj)
    {
        _vp = viewProj;
        flush();
        _driver->setUniform("mat", _vp);
    }

    void STDRenderer::setViewProjTransform(const Matrix& viewProj)
    {
        setViewProj(viewProj);
    }

    void STDRenderer::pushShaderSetHook(ShaderProgramChangedHook* hook)
    {
        _sphookLast->next = hook;
        hook->prev = _sphookLast;
        _sphookLast = hook;
    }

    void STDRenderer::popShaderSetHook()
    {
        _sphookLast = _sphookLast->prev;
        _sphookLast->next = 0;
    }

    void STDRenderer::begin()
    {
        OX_ASSERT(_verticesData.empty() == true);
        _verticesData.clear();
        _transform.identity();

        MaterialX::null->apply();

        xbegin();

        current = this;
    }

    void STDRenderer::end()
    {
        flush();

        if (_prevRT)
        {
            _driver->setRenderTarget(_prevRT);
            _prevRT = 0;
        }

    }

    void STDRenderer::setVertexDeclaration(const VertexDeclaration* decl)
    {
        if (_vdecl != decl)
            flush();
        _vdecl = decl;
    }

    void STDRenderer::addVertices(const void* data, unsigned int size)
    {
        xaddVertices(data, size);
        checkDrawBatch();
    }

    void STDRenderer::xaddVertices(const void* data, unsigned int size)
    {
        _verticesData.insert(_verticesData.end(), (const unsigned char*)data, (const unsigned char*)data + size);
    }

    void STDRenderer::checkDrawBatch()
    {
        if (_verticesData.size() / sizeof(_vdecl->size) >= maxVertices)
            flush();
    }

    Matrix makeViewMatrix(int w, int h, bool flipU)
    {
        //printf("s1\n");
        Matrix view, scale, tr;
        float offset = 0.5f;

        offset = 0;

        Matrix::translation(tr, Vector3(-(float)w / 2.0f - offset, (flipU ? -1.0f : 1.0f) * (float)h / 2.0f + offset, 0.0f));
        Matrix::scaling(scale, Vector3(1.0f, flipU ? 1.0f : -1.0f, 1.0f));

        view = scale * tr;

        return view;
    }




    bool checkT2P(const Rect& viewport, const Matrix& vp, const vertexPCT2* v1, const vertexPCT2* v2, int w, int h)
    {
        Vector3 p1(v1->x, v1->y, 0);
        Vector3 p2(v2->x, v2->y, 0);

        p1 = vp.transformVec3(p1);
        p2 = vp.transformVec3(p2);

        Vector2 half = viewport.getSize().cast<Vector2>() / 2;
        p1.x = p1.x * half.x + half.x;
        p1.y = p1.y * half.y + half.y;

        p2.x = p2.x * half.x + half.x;
        p2.y = p2.y * half.y + half.y;

        Vector2 tc1(v1->u, v1->v);
        Vector2 tc2(v2->u, v2->v);
        Vector3 dp_ = p1 - p2;
        Vector2 dp(dp_.x, dp_.y);
        dp.x = scalar::abs(dp.x);
        dp.y = scalar::abs(dp.y);

        Vector2 dtc = tc1 - tc2;
        dtc.x = scalar::abs(dtc.x) * w;
        dtc.y = scalar::abs(dtc.y) * h;

        const float EPS = 0.05f;

        Vector2 d = dp - dtc;
        if (scalar::abs(d.x) >= EPS || scalar::abs(d.y) >= EPS)
            return false;

        p1.x = scalar::abs(p1.x);
        p1.y = scalar::abs(p1.y);

        if (scalar::abs(p1.x - int(p1.x + EPS)) > EPS ||
                scalar::abs(p1.y - int(p1.y + EPS)) > EPS)
            return false;

        return true;
    }



    STDRenderer::STDRenderer(IVideoDriver* driver) : _driver(driver), _vdecl(0), _uberShader(0)
    {
        if (!driver)
            driver = IVideoDriver::instance;

        _driver = driver;
        _vp.identity();

        _vdecl = _driver->getVertexDeclaration(vertexPCT2::FORMAT);

        _uberShader = &uberShader;
        _transform.identity();
        _baseShaderFlags = 0;

        _sphookFirst = this;
        _sphookLast  = this;

        hook = [ = ]()
        {
            _driver->setUniform("mat", _vp);
        };
    }


    template <class T>
    void append(std::vector<unsigned char>& buff, const T& t)
    {
        const unsigned char* ptr = (const unsigned char*)&t;
        buff.insert(buff.end(), ptr, ptr + sizeof(t));
    }

    bool _showTexel2PixelErrors = false;

#ifdef OXYGINE_DEBUG_T2P
    void STDRenderer::showTexel2PixelErrors(bool show)
    {
        _showTexel2PixelErrors = show;
    }
#endif

    void STDRenderer::swapVerticesData(STDRenderer& r)
    {
        std::swap(_verticesData, r._verticesData);
    }

    void STDRenderer::swapVerticesData(std::vector<unsigned char>& data)
    {
        std::swap(data, _verticesData);
    }


    void STDRenderer::setTransform(const Transform& tr)
    {
        _transform = tr;
    }

    void STDRenderer::xbegin()
    {
    }

    void STDRenderer::begin(spNativeTexture nt, const Rect* viewport)
    {
        OX_ASSERT(_prevRT == 0);
        _prevRT = _driver->getRenderTarget();
        _driver->setRenderTarget(nt);

        Rect vp;
        if (!viewport)
        {
            vp = Rect(0, 0, nt->getWidth(), nt->getHeight());
            viewport = &vp;
        }
        _driver->setViewport(*viewport);


        initCoordinateSystem(viewport->getWidth(), viewport->getHeight(), true);
        begin();
    }


    void STDRenderer::addQuad(const Color& clr, const RectF& srcRect, const RectF& destRect)
    {
        Color color = clr.premultiplied();

        vertexPCT2 v[4];
        fillQuadT(v, srcRect, destRect, _transform, color.rgba());


#ifdef OXYGINE_DEBUG_T2P
        if (_showTexel2PixelErrors)
        {
            spNativeTexture base = rsCache().getTexture(UberShaderProgram::SAMPLER_BASE);
            if (base != white)
            {
                Rect viewport;
                _driver->getViewport(viewport);

                bool t = checkT2P(viewport, _vp, &v[0], &v[3], base->getWidth(), base->getHeight());
                if (!t)
                {
                    float c = (sinf((float)getTimeMS() / 200 + v[0].x * v[0].y) + 1) / 2.0f;
                    Color b = lerp(Color(rand() % 255, rand() % 255, rand() % 255, 255), color, c);
                    fillQuadT(v, srcRect, destRect, _transform, b.rgba());
                }
            }
        }
#endif

        addVertices(v, sizeof(v));
    }


    void STDRenderer::setShaderFlags(unsigned int flags)
    {
        ShaderProgram* sp = _uberShader->getShaderProgram(_baseShaderFlags | flags);
        setShader(sp);
    }

    void STDRenderer::flush()
    {
        size_t indices = (_verticesData.size() / sizeof(vertexPCT2) * 3) / 2;
        if (!indices)
            return;

        _driver->draw(IVideoDriver::PT_TRIANGLES, _vdecl,
                      &_verticesData.front(), (unsigned int)_verticesData.size(),
                      &STDRenderer::indices16.front(), (unsigned int)indices);

        _verticesData.clear();
    }


    void STDRenderer::setUberShaderProgram(UberShaderProgram* pr)
    {
        if (_uberShader == pr)
            return;

        _uberShader = pr;
    }

    void STDRenderer::setBaseShaderFlags(unsigned int fl)
    {
        _baseShaderFlags = fl;
    }

}
