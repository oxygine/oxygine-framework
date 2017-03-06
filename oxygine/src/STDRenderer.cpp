#include "STDRenderer.h"
#include "core/UberShaderProgram.h"
#include "core/VertexDeclaration.h"
#include "STDMaterial.h"
#include "Image.h"
#include "core/file.h"
#include "core/ZipFileSystem.h"
#include "core/system_data.h"
#include "math/Rect.h"
#include "Actor.h"
#include "Image.h"
#include "core/ImageDataOperations.h"
#include "AnimationFrame.h"
#include "core/VertexDeclaration.h"
#include "core/gl/VideoDriverGLES20.h"

#include "core/UberShaderProgram.h"

#include "RenderState.h"
#include "VisualStyle.h"


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
        drawBatch();
    }

    void STDRenderer::cleanup()
    {
        _cleanup();
        _vertices.clear();
    }

    void STDRenderer::setShader(ShaderProgram* prog)
    {
        if (prog != _program)
        {
            _driver->setShaderProgram(prog);
            _driver->setUniform("mat", _vp);
        }
        _program = prog;
    }


    void STDRenderer::_drawBatch()
    {
        size_t count = _vertices.size() / _vdecl->size;
        size_t indices = (count * 3) / 2;

        getDriver()->draw(IVideoDriver::PT_TRIANGLES, _vdecl, &_vertices.front(), (unsigned int)count, &indices16.front(), (unsigned int)indices);

        _vertices.clear();
    }

    void STDRenderer::drawBatch()
    {
        if (!_vertices.empty())
        {
            preDrawBatch();
            _drawBatch();
        }
    }

    void STDRenderer::initCoordinateSystem(int width, int height, bool flipU)
    {
        Matrix view = makeViewMatrix(width, height, flipU);
        Matrix proj;
        //initialize projection matrix
        Matrix::orthoLH(proj, (float)width, (float)height, 0, 1);

        Matrix vp = view * proj;
        setViewProjTransform(vp);
    }

    IVideoDriver* STDRenderer::getDriver()
    {
        return _driver;
    }

    void STDRenderer::setDriver(IVideoDriver* driver)
    {
        _driver = driver;
    }

    void STDRenderer::setViewProj(const Matrix& viewProj)
    {
        _vp = viewProj;
        if (_drawing)
        {
            drawBatch();
        }


        _driver->setUniform("mat", _vp);
    }

    void STDRenderer::resetSettings()
    {
        _resetSettings();
        _driver->setState(IVideoDriver::STATE_BLEND, 0);
        _program = 0;
    }

    void STDRenderer::begin()
    {
        OX_ASSERT(!_drawing);
        OX_ASSERT(_vertices.empty() == true);
        _program = 0;
        _vertices.clear();
        _transform.identity();
        resetSettings();

        _begin();

        _drawing = true;

        current = this;
    }

    void STDRenderer::end()
    {
        drawBatch();

        if (_prevRT)
        {
            _driver->setRenderTarget(_prevRT);
            _prevRT = 0;
        }

        //current = 0;

        _drawing = false;
    }

    void STDRenderer::setVertexDeclaration(const VertexDeclaration* decl)
    {
        if (_vdecl != decl)
            drawBatch();
        _vdecl = decl;
    }

    void STDRenderer::addVertices(const void* data, unsigned int size)
    {
        _addVertices(data, size);
        _checkDrawBatch();
    }

    void STDRenderer::_addVertices(const void* data, unsigned int size)
    {
        _vertices.insert(_vertices.end(), (const unsigned char*)data, (const unsigned char*)data + size);
    }

    void STDRenderer::_checkDrawBatch()
    {
        if (_vertices.size() / sizeof(_vdecl->size) >= maxVertices)
            drawBatch();
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



    STDRenderer::STDRenderer(IVideoDriver* driver) : _driver(driver), _program(0), _vdecl(0), _previous(0), _uberShader(0), _shaderFlags(0), _blend(blend_disabled)
    {
        if (!driver)
            driver = IVideoDriver::instance;

        if (driver)
            setDriver(driver);

        _vp.identity();

        _vdecl = _driver->getVertexDeclaration(vertexPCT2::FORMAT);

        _uberShader = &uberShader;
        _transform.identity();
        _drawing = false;
    }

    void STDRenderer::setBlendMode(blend_mode blend)
    {
        if (_blend != blend)
        {
            drawBatch();

            if (blend == 0)
            {
                _driver->setState(IVideoDriver::STATE_BLEND, 0);
            }
            else
            {
                IVideoDriver::BLEND_TYPE src  = static_cast<IVideoDriver::BLEND_TYPE>(blend >> 16);
                IVideoDriver::BLEND_TYPE dest = static_cast<IVideoDriver::BLEND_TYPE>(blend & 0xFFFF);
                _driver->setBlendFunc(src, dest);
                _driver->setState(IVideoDriver::STATE_BLEND, 1);
            }
            _blend = blend;
        }
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


    void STDRenderer::setTexture(const spNativeTexture& base_, const spNativeTexture& alpha, bool basePremultiplied)
    {
        _renderTextureHook(base_);
        _renderTextureHook(alpha);

        spNativeTexture base = base_;
        if (base == 0 || base->getHandle() == 0)
            base = white;

        unsigned int shaderFlags = _shaderFlags;

        if (basePremultiplied)
            shaderFlags &= ~UberShaderProgram::ALPHA_PREMULTIPLY;
        else
            shaderFlags |= UberShaderProgram::ALPHA_PREMULTIPLY;

        if (alpha)
            shaderFlags |= UberShaderProgram::SEPARATE_ALPHA;
        else
            shaderFlags &= ~UberShaderProgram::SEPARATE_ALPHA;

        //##ifdef OX_DEBUG
#if 0
        if (_base != base) { OX_ASSERT(_alpha != alpha); }
        else { OX_ASSERT(_alpha == alpha); }
#endif //OX_DEBUG

        //no reason to check changed alpha because it is in pair with base
        if (_base != base || /*_alpha != alpha || */_shaderFlags != shaderFlags)
        {
            drawBatch();
        }

        _shaderFlags = shaderFlags;

        _base = base;
        _alpha = alpha;
    }

    void STDRenderer::setTexture(const spNativeTexture& base_, bool basePremultiplied)
    {
        spNativeTexture base = base_;
        if (base == 0 || base->getHandle() == 0)
            base = white;

        unsigned int shaderFlags = _shaderFlags;

        if (basePremultiplied)
            shaderFlags &= ~UberShaderProgram::ALPHA_PREMULTIPLY;
        else
            shaderFlags |= UberShaderProgram::ALPHA_PREMULTIPLY;

        shaderFlags &= ~UberShaderProgram::SEPARATE_ALPHA;

        if (_base != base || _shaderFlags != shaderFlags)
            drawBatch();

        _shaderFlags = shaderFlags;

        _base = base;
        _alpha = 0;
    }

    void STDRenderer::_begin()
    {
        _base = 0;
        _alpha = 0;
        _blend = blend_disabled;
    }

    void STDRenderer::begin(spNativeTexture nt, const Rect* viewport)
    {
        OX_ASSERT(!_drawing);
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

    void STDRenderer::_resetSettings()
    {
        _blend = blend_disabled;
    }

    void STDRenderer::_cleanup()
    {
        _base = 0;
        _alpha = 0;
    }

    void STDRenderer::preDrawBatch()
    {
        ShaderProgram* prog = _uberShader->getShaderProgram(_shaderFlags);
        setShader(prog);


        _uberShader->apply(_driver, _base, _alpha);

        UberShaderProgramBase::ShaderUniformsCallback cb = _uberShader->getShaderUniformsCallback();
        if (cb)
        {
            cb(_driver, prog);
        }
    }

    void STDRenderer::draw(const Color& clr, const RectF& srcRect, const RectF& destRect)
    {
        Color color = clr;
        if (_blend == blend_premultiplied_alpha)
            color = color.premultiplied();

        vertexPCT2 v[4];
        fillQuadT(v, srcRect, destRect, _transform, color.rgba());


#ifdef OXYGINE_DEBUG_T2P
        if (_base != white && _showTexel2PixelErrors)
        {
            Rect viewport;
            _driver->getViewport(viewport);

            bool t = checkT2P(viewport, _vp, &v[0], &v[3], _base->getWidth(), _base->getHeight());
            if (!t)
            {
                float c = (sinf((float)getTimeMS() / 200 + v[0].x * v[0].y) + 1) / 2.0f;
                Color b = lerp(Color(rand() % 255, rand() % 255, rand() % 255, 255), color, c);
                fillQuadT(v, srcRect, destRect, _transform, b.rgba());
            }
        }
#endif

        addVertices(v, sizeof(v));
    }

    void STDRenderer::setUberShaderProgram(UberShaderProgram* pr)
    {
        if (_uberShader != pr)
        {
            drawBatch();
        }

        _uberShader = pr;
    }

    void STDRenderer::beginSDFont(float contrast, float offset, const Color& outlineColor, float outlineOffset)
    {
        if (_alpha)
        {
            drawBatch();
            _shaderFlags &= ~UberShaderProgram::SEPARATE_ALPHA;
            _alpha = 0;
        }

        unsigned int shaderFlags = _shaderFlags;
        shaderFlags |= UberShaderProgram::SDF;

        if (outlineOffset < offset)
            shaderFlags |= UberShaderProgram::SDF_OUTLINE;

        if (_shaderFlags != shaderFlags)
        {
            drawBatch();
        }

        _shaderFlags = shaderFlags;

        ShaderProgram* prog = _uberShader->getShaderProgram(_shaderFlags);
        setShader(prog);

        Vector4 c = outlineColor.toVector();
        _driver->setUniform("sdf_outline_color", c);

        c = Vector4(offset, contrast, outlineOffset, contrast);
        _driver->setUniform("sdf_params", c);
    }

    void STDRenderer::endSDFont()
    {
        drawBatch();
        _shaderFlags &= ~(UberShaderProgram::SDF | UberShaderProgram::SDF_OUTLINE);

        ShaderProgram* prog = _uberShader->getShaderProgram(_shaderFlags);
        setShader(prog);
    }

    void STDRenderer::beginElementRendering(bool basePremultiplied)
    {
        if (_alpha)
        {
            drawBatch();
            _shaderFlags &= ~UberShaderProgram::SEPARATE_ALPHA;
            _alpha = 0;
        }

        unsigned int shaderFlags = _shaderFlags;

        if (basePremultiplied)
            shaderFlags &= ~UberShaderProgram::ALPHA_PREMULTIPLY;
        else
            shaderFlags |= UberShaderProgram::ALPHA_PREMULTIPLY;

        if (_shaderFlags != shaderFlags)
        {
            drawBatch();
        }
    }

    void STDRenderer::drawElement(const spNativeTexture& texture, unsigned int color, const RectF& src, const RectF& dest)
    {
        if (_base != texture)
        {
            drawBatch();
            _base = texture;
        }

        vertexPCT2 v[4];
        fillQuadT(v, src, dest, _transform, color);
        addVertices(v, sizeof(v));
    }
}
