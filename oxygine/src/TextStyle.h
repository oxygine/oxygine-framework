#pragma once
#include "oxygine_include.h"
#include "math/Color.h"
#include <string>

namespace oxygine
{
    class Font;
    class ResFont;

    class TextStyle
    {
    public:
        enum HorizontalAlign
        {
            HALIGN_DEFAULT,
            HALIGN_LEFT,
            HALIGN_MIDDLE,
            HALIGN_CENTER = HALIGN_MIDDLE,//HALIGN_CENTER deprecated
            HALIGN_RIGHT
        };

        enum VerticalAlign
        {
            VALIGN_DEFAULT,
            VALIGN_BASELINE,
            VALIGN_TOP,
            VALIGN_MIDDLE,
            VALIGN_BOTTOM
        };


        TextStyle(): font(0),
            hAlign(HALIGN_DEFAULT),
            vAlign(VALIGN_DEFAULT),
            linesOffset(0),
            kerning(0),
            multiline(false),
            fontSize(0),
            breakLongWords(false),
            outline(0.0f),
            outlineColor(Color::Black),
            weight(0.5f),
            baselineScale(1.0f),
            options(0) {}

        const ResFont* font;

        HorizontalAlign hAlign;
        VerticalAlign vAlign;

        int linesOffset;//vertical distance offset between lines
        int kerning;//horizontal distance
        unsigned int options;//additional flags could be used for generating custom glyphs
        bool multiline;
        bool breakLongWords;//works with multiline flag. breakLongWords = false doesn't allow to break too long words
        Color color;


        union
        {
            int fontSize;
            int fontSize2Scale;//DEPRECATED, use fontSize
        };


        float outline;//works only with SD fonts, disabled by default = 0.0f, 0.5 - max outline
        Color outlineColor;//works only with SD fonts
        float weight;//works only with SD fonts, font weight, default = 0.5f,  0.0 - bold, 1.0 - thin
        float baselineScale;//baseline distance multiplier



        TextStyle withFont(const ResFont* f) const { TextStyle st = *this; st.font = f; return st; }

        TextStyle alignTop() const { TextStyle st = *this; st.vAlign = VALIGN_TOP; return st; }
        TextStyle alignBottom() const { TextStyle st = *this; st.vAlign = VALIGN_BOTTOM; return st; }
        TextStyle alignBaseline() const { TextStyle st = *this; st.vAlign = VALIGN_BASELINE; return st; }
        TextStyle alignMiddleV() const { TextStyle st = *this; st.vAlign = VALIGN_MIDDLE; return st; }

        TextStyle alignLeft() const { TextStyle st = *this; st.hAlign = HALIGN_LEFT; return st; }
        TextStyle alignRight() const { TextStyle st = *this; st.hAlign = HALIGN_RIGHT; return st; }
        TextStyle alignMiddleH() const { TextStyle st = *this; st.hAlign = HALIGN_MIDDLE; return st; }

        TextStyle alignMiddle() const { TextStyle st = *this; st.vAlign = VALIGN_MIDDLE; st.hAlign = HALIGN_MIDDLE; return st; }

        TextStyle withHOffset(int offset) const { TextStyle st = *this; st.linesOffset = offset; return st; }
        TextStyle withBaselineScale(float s) const { TextStyle st = *this; st.baselineScale = s; return st; }

        TextStyle withKerning(int kerning) const { TextStyle st = *this; st.kerning = kerning; return st; }
        TextStyle withMultiline(bool multiline = true) const { TextStyle st = *this; st.multiline = multiline; return st; }
        TextStyle withColor(const Color& color) const { TextStyle st = *this; st.color = color; return st; }

        TextStyle withFontSize(int size) const { TextStyle st = *this; st.fontSize = size; return st; }

        TextStyle withOutline(float outline) const { TextStyle st = *this; st.outline = outline; return st; }
        TextStyle withOutlineColor(const Color& color) const { TextStyle st = *this; st.outlineColor = color; return st; }
        TextStyle withWeight(float weight) const { TextStyle st = *this; st.weight = weight; return st; }
        TextStyle withOptions(unsigned int opt) const { TextStyle st = *this; st.options = opt; return st; }
    };

    std::string dumpStyle(const TextStyle& s, bool onlydiff);
}