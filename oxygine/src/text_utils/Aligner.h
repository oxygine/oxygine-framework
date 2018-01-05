#pragma once
#include "oxygine-include.h"
#include <vector>
#include "math/Rect.h"
#include "math/Color.h"
#include "TextStyle.h"
#include "Font.h"
#include "core/NativeTexture.h"

namespace oxygine
{
    struct glyph;
    class Font;

    namespace text
    {
        struct Symbol
        {
            Symbol(): x(0), y(0), code(0) {}

            float x, y;
            int code;
            glyph gl;
            RectF destRect;
        };

        class Aligner
        {
        public:
            Aligner(const TextStyle& style, const Font* font, float gscale, const Vector2& size);
            ~Aligner();


            const TextStyle& getStyle() const {return style;}
            float getScale() const;

            void begin();
            void end();

            int putSymbol(Symbol& s);
            void nextLine();


            TextStyle style;
            RectF bounds;
            float width;
            float height;
            int options;


            const Font* _font;

        private:
            float getLineWidth()const;
            float getLineSkip()const;


            typedef std::vector<Symbol*> line;

            void _alignLine(line& ln);
            float _alignX(float rx);
            float _alignY(float ry);
            void _nextLine(line& ln);

            float _scale;
            float _x, _y;
            line _line;
            float _lineWidth;
            float _lineSkip;
        };
    }
}