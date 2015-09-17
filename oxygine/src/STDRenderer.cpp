#include "STDRenderer.h"
#include "core/UberShaderProgram.h"
#include "core/VertexDeclaration.h"
#include "RenderState.h"

namespace oxygine
{
    STDRenderer::STDRenderer(IVideoDriver* driver) : Renderer(driver), _uberShader(0), _shaderFlags(0), _blend(blend_disabled)
    {
        _uberShader = &uberShader;
    }

    void STDRenderer::setBlendMode(blend_mode blend)
    {
        if (_blend != blend)
        {
            drawBatch();

            switch (blend)
            {
                case blend_disabled:
                    _driver->setState(IVideoDriver::STATE_BLEND, 0);
                    break;
                case blend_premultiplied_alpha:
                    _driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
                    break;
                case blend_alpha:
                    _driver->setBlendFunc(IVideoDriver::BT_SRC_ALPHA, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
                    break;
                case blend_add:
                    _driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
                    break;
                case blend_multiply:
                    _driver->setBlendFunc(IVideoDriver::BT_DST_COLOR, IVideoDriver::BT_ONE_MINUS_SRC_ALPHA);
                    break;
                //case blend_sub:
                //_driver->setBlendFunc(IVideoDriver::BT_ONE, IVideoDriver::BT_ONE);
                //glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
                //  break;
                default:
                    OX_ASSERT(!"unknown blend");
            }

            if (_blend == blend_disabled)
            {
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

    void STDRenderer::setTexture(spNativeTexture base, spNativeTexture alpha, bool basePremultiplied)
    {
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

    void STDRenderer::_begin()
    {
        _base = 0;
        _alpha = 0;
        _blend = blend_disabled;
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
        ShaderProgram* prog = _uberShader->getShaderProgram(_shaderFlags)->program;
        setShader(prog);

        _driver->setTexture(UberShaderProgram::SAMPLER_BASE, _base);

        if (_alpha)
            _driver->setTexture(UberShaderProgram::SAMPLER_ALPHA, _alpha);

        UberShaderProgramBase::ShaderUniformsCallback cb = _uberShader->getShaderUniformsCallback();
        if (cb)
        {
            cb(_driver, prog);
        }
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




    void STDRenderer::draw(const RState* rs, const Color& clr, const RectF& srcRect, const RectF& destRect)
    {
        Color color = clr;
        color.a = (int(color.a) * rs->alpha) / 255;
        if (_blend == blend_premultiplied_alpha)
            color = color.premultiplied();

        vertexPCT2 v[4];
        fillQuadT(v, srcRect, destRect, rs->transform, color.rgba());


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
                fillQuadT(v, srcRect, destRect, rs->transform, b.rgba());
            }
        }
#endif

        addVertices(v, sizeof(v));
    }

    void STDRenderer::setUberShaderProgram(UberShaderProgramBase* pr)
    {
        if (_uberShader != pr)
        {
            drawBatch();
        }

        _uberShader = pr;
    }

    PrimitiveRenderer::PrimitiveRenderer(STDRenderer* r) : _renderer(r)
    {

    }

    void PrimitiveRenderer::setTexture(spNativeTexture texture)
    {
        //_renderer->setTexture(texture);
    }

    void PrimitiveRenderer::setBlendMode(blend_mode mode)
    {
        //_renderer->setBlendMode(mode);
    }

    void PrimitiveRenderer::draw(const void* data, int size, bvertex_format format, bool worldTransform)
    {
        //_renderer->addVertices()
    }

    TextRenderer2::TextRenderer2(STDRenderer* r) : _renderer(r)
    {
        //_renderer->setBlendMode(Renderer::getPremultipliedAlphaRender() ? blend_premultiplied_alpha : blend_alpha);
    }

    void TextRenderer2::draw(const AffineTransform& tr, spNativeTexture texture, unsigned int color, const RectF& srcRect, const RectF& destRect)
    {
        if (_texture != texture)
        {
            _renderer->setTexture(texture, 0, true);
            _texture = texture;
        }

        vertexPCT2 v[4];
        fillQuadT(v, srcRect, destRect, tr, color);

#ifdef OXYGINE_DEBUG_T2P
        if (_showTexel2PixelErrors)
        {
            Rect viewport;
            _renderer->getDriver()->getViewport(viewport);

            bool t = checkT2P(viewport, _renderer->getViewProjection(), &v[0], &v[3], _texture->getWidth(), _texture->getHeight());
            if (!t)
            {
                float c = (sinf((float)getTimeMS() / 200 + v[0].x * v[0].y) + 1) / 2.0f;
                Color b = lerp(Color(rand() % 255, rand() % 255, rand() % 255, 255), color, c);
                fillQuadT(v, srcRect, destRect, tr, b.rgba());
            }
        }
#endif

        _renderer->addVertices(v, sizeof(v));
    }
}
