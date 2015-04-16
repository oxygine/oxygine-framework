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

    Font::Font() : _size(0), _baselineDistance(0), _scale(1.0f)
    {
    }

    Font::~Font()
    {
    }

    void Font::init(const char* name, int realSize, int baselineDistance, int lineHeight)
    {
        setName(name);
        _size = realSize;
        _baselineDistance = baselineDistance;
        _lineHeight = lineHeight;
        _glyphs.reserve(200);
    }

    void Font::addGlyph(const glyph& gl)
    {
        _glyphs.push_back(gl);
    }

    bool glyphFindPred(const glyph& g, int code)
    {
        return g.ch < code;
    }

    bool glyphsComparePred(const glyph& ob1, const glyph& ob2)
    {
        return ob1.ch < ob2.ch;
    }

    void Font::sortGlyphs()
    {
        std::sort(_glyphs.begin(), _glyphs.end(), glyphsComparePred);
    }

    const glyph* Font::getGlyph(int ch) const
    {
        glyphs::const_iterator it = std::lower_bound(_glyphs.begin(), _glyphs.end(), ch, glyphFindPred);
        if (it != _glyphs.end())
        {
            const glyph& g = *it;
            if (g.ch == ch)
                return &g;
        }

        return 0;
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

    int Font::getLineHeight() const
    {
        return _lineHeight;
    }
}