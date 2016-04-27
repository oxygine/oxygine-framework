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
            fontSize(0),
            breakLongWords(false),
            outline(0) {}

        const Font* font;

        HorizontalAlign hAlign;
        VerticalAlign vAlign;

        int linesOffset;//vertical distance offset between lines
        int kerning;//horizontal distance
        bool multiline;
        bool breakLongWords;//works with multiline flag. breakLongWords = false doesn't allow to break too long words
        Color color;

        Color outlineColor;//works only with SD fonts
        float outline;//works only with SD fonts

        union
        {
            int fontSize;
            int fontSize2Scale;//DEPRECATED, use fontSize
        };
    };

    std::string dumpStyle(const TextStyle& s, bool onlydiff);
}