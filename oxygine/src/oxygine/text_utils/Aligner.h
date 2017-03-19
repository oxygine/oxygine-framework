#pragma once
#include "../oxygine-include.h"
#include <vector>
#include "../math/Rect.h"
#include "../math/Color.h"
#include "../TextStyle.h"
#include "../Font.h"
#include "../core/NativeTexture.h"
#include "../MaterialX.h"

namespace oxygine
{
    struct glyph;
    class Font;


    namespace text
    {
        struct Symbol
        {
            Symbol(): x(0), y(0), code(0) {}

            short x, y;
            int code;
            glyph gl;
            RectF destRect;
            spMaterialX materialX;
        };

        class Aligner
        {
        public:
            Aligner(const TextStyle& style, const STDMatData& md, const Font* font, float gscale, const Vector2& size);
            ~Aligner();


            const TextStyle& getStyle() const {return style;}
            float getScale() const;

            void begin();
            void end();

            int putSymbol(Symbol& s);
            void nextLine();


            TextStyle style;
            Rect bounds;
            int width;
            int height;
            int options;

            const STDMatData& data;

            const Font* _font;

        private:
            int getLineWidth()const;
            int getLineSkip()const;


            typedef std::vector<Symbol*> line;

            void _alignLine(line& ln);
            int _alignX(int rx);
            int _alignY(int ry);
            void _nextLine(line& ln);

            float _scale;
            int _x, _y;
            line _line;
            int _lineWidth;
            int _lineSkip;
        };
    }
}