#pragma once
#include "oxygine_include.h"
#include "math/Color.h"
#include <string>

namespace oxygine
{
    class Font;
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
            fontSize2Scale(0),
            breakLongWords(false) {}

        const Font* font;

        HorizontalAlign hAlign;
        VerticalAlign vAlign;

        int linesOffset;//distance offset between lines
        int kerning;
        bool multiline;
        bool breakLongWords;//works with multiline flag. breakLongWords = false doesn't allow to break too long words
        Color color;
        int fontSize2Scale;
    };

    std::string dumpStyle(const TextStyle& s, bool onlydiff);
}