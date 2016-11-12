#pragma once
#include "oxygine_include.h"
#include "core/Object.h"
#include "math/Rect.h"
#ifdef __S3E__
#include <set>
#else
#include <unordered_set>
#endif
namespace oxygine
{
    DECLARE_SMART(NativeTexture, spNativeTexture);

    typedef unsigned int glyphOptions;

    struct glyph
    {
        RectF src;

        int ch;
        glyphOptions opt;

        short sw;
        short sh;

        short offset_x;
        short offset_y;

        short advance_x;
        short advance_y;

        spNativeTexture texture;

        bool operator == (const glyph& r) const {return ch == r.ch && opt == r.opt;}
#ifdef __S3E__
        bool operator < (const glyph& r) const { return ch < r.ch; }
#endif
    };

    struct GlyphHasher
    {
        std::size_t operator()(const glyph& k) const
        {
            return std::hash<int>()(k.ch + k.opt);
        }
    };


    class Font: public ObjectBase
    {
    public:
        Font();
        ~Font();

        void init(const char* name, int size, int baselineDistance, int lineHeight, bool sdf = false);

        void addGlyph(const glyph& g);
        void sortGlyphs() {}

        void setScale(float scale) { _scale = scale; }
        void setBaselineDistance(int d) { _baselineDistance = d; }

        const glyph*    getGlyph(int code, const glyphOptions& opt) const;
        int             getBaselineDistance() const;
        int             getSize() const;
        float           getScale() const;
        bool            isSDF() const;

    protected:
        const glyph* findGlyph(int code, const glyphOptions& opt) const;

        virtual bool loadGlyph(int code, glyph&, const glyphOptions& opt) { return false; }

#ifdef __S3E__
        typedef std::set<glyph> glyphs;
#else
        typedef std::unordered_set<glyph, GlyphHasher> glyphs;
#endif
        glyphs _glyphs;

        float _scale;
        bool _sdf;

        int _size;
        int _baselineDistance;
    };
}