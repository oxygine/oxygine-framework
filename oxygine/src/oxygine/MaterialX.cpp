#include "MaterialX.h"
#include "STDRenderer.h"
#include "core/UberShaderProgram.h"
#include "STDRenderer.h"

namespace oxygine
{
    spMaterialX MaterialX::current;

    bool STDMatData::cmp(const STDMatData& b) const
    {
        if (_base != b._base)
            return false;
        if (_alpha != b._alpha)
            return false;
        if (_blend != b._blend)
            return false;
        if (_flags != b._flags)
            return false;
        if (_uberShader != b._uberShader)
            return false;
        return true;
    }

    STDMatData::STDMatData() : _blend(blend_premultiplied_alpha), _flags(0), _uberShader(&STDRenderer::uberShader)
    {

    }

    void STDMatData::init(size_t& hash) const
    {
        hash_combine(hash, _base.get());
        hash_combine(hash, _alpha.get());
        hash_combine(hash, (int)_blend);
        hash_combine(hash, _flags);
        hash_combine(hash, _uberShader);
    }

    void STDMatData::apply()
    {
        STDRenderer* r = STDRenderer::getCurrent();
        r->setUberShaderProgram(_uberShader);
        r->setShaderFlags(_flags);
        r->setTextureNew(UberShaderProgram::SAMPLER_BASE, _base);
        r->setTextureNew(UberShaderProgram::SAMPLER_ALPHA, _alpha);
        r->setBlendMode(_blend);
    }

    void STDMatData::flush()
    {
        STDRenderer::getCurrent()->drawBatch();
    }

    MaterialX::MaterialX(const MaterialX& other)
    {
        _hash = other._hash;
        _compare = other._compare;
    }

    MaterialX::MaterialX(compare cmp) : _hash(0), _compare(cmp)
    {

    }

    MaterialX::MaterialX() : _hash(0)
    {

    }

    void MaterialX::render(const AffineTransform &tr, const Color& c, const RectF& src, const RectF& dest)
    {
        STDRenderer::getCurrent()->setTransform(tr);
        STDRenderer::getCurrent()->draw(c, src, dest);
    }

    void MaterialX::render(const Color& c, const RectF& src, const RectF& dest)
    {
        STDRenderer::getCurrent()->draw(c, src, dest);
    }

    void MaterialX::apply()
    {
        if (current != this)
        {
            if (current)
                current->flush();
            xapply();
            current = this;
        }
    }


    void MaterialX::flush()
    {
        xflush();
    }

    oxygine::MaterialX& MaterialX::operator=(const MaterialX& r)
    {
        _compare = r._compare;
        _hash = r._hash;
        return *this;
    }

}