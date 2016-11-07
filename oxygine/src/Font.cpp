#include "Font.h"
#include "core/NativeTexture.h"

namespace oxygine
{
    /*
    int key2hash(int g)
    {
        int k = g;
        if ((g & 0xC0) == 0xC0)
        {
            k = k >> 8;
        }
        k = k & 0x3f;
        //log::messageln("key: %d  hash: %d", g, k);
        return k;
        //return g;
    }
    */

    Font::Font() : _size(0), _baselineDistance(0), _scale(1.0f), _sdf(false)
    {
    }

    Font::~Font()
    {
    }

    void Font::init(const char* name, int realSize, int baselineDistance, int lineHeight, bool sdf)
    {
        setName(name);
        _sdf = sdf;
        _size = realSize;
        _baselineDistance = baselineDistance;
    }

    void Font::addGlyph(const glyph& gl)
    {
        _glyphs.insert(gl);
    }

    bool glyphFindPred(const glyph& g, int code)
    {
        return g.ch < code;
    }

    bool glyphsComparePred(const glyph& ob1, const glyph& ob2)
    {
        return ob1.ch < ob2.ch;
    }

    const glyph* Font::findGlyph(int code, const glyphOptions& opt) const
    {
        glyph g;
        g.ch = code;
        g.opt = opt;
        glyphs::const_iterator it = _glyphs.find(g);
        if (it != _glyphs.end())
        {
            return &(*it);
        }

        return 0;
    }

    const glyph* Font::getGlyph(int code, const glyphOptions& opt) const
    {
        const glyph* g = findGlyph(code, opt);
        if (g)
            return g;

        glyph gl;
        Font* fn = const_cast<Font*>(this);
        if (fn->loadGlyph(code, gl, opt))
        {
            fn->_glyphs.insert(gl);
            g = findGlyph(code, opt);
            OX_ASSERT(g);
        }

        return g;
    }

    int Font::getBaselineDistance() const
    {
        return _baselineDistance;
    }

    int Font::getSize() const
    {
        return _size;
    }

    float Font::getScale() const
    {
        return _scale;
    }

    bool Font::isSDF() const
    {
        return _sdf;
    }
}