#pragma once
#include "test.h"
#include "TextField.h"
#include "utils/stringUtils.h"

class TweenText
{
public:
    typedef TextField type;

    TweenText(const string& text)
    {
        //convert utf8 string to wide string
        _text = utf8tows(text.c_str());
    }

    void init(TextField& actor) {}
    void update(TextField& actor, float p, const UpdateState& us)
    {
        int v = lerp<int>(0, _text.size(), p);
        wstring res = _text.substr(0, v) + L"<div c = '0x00000000'>" + _text.substr(v, _text.size()) + L"</div>";

        //convert back to utf8 string
        string t = ws2utf8(res.c_str());
        actor.setHtmlText(t);
    }

private:
    wstring _text;

};


class TestTweenText : public Test
{
public:
    TestTweenText()
    {
        spTextField text = new TextField;
        content->addChild(text);

        TextStyle st;
        st.font = resourcesUI.getResFont("big")->getFont();
        st.vAlign = TextStyle::VALIGN_MIDDLE;
        st.hAlign = TextStyle::HALIGN_MIDDLE;
        st.multiline = true;
        text->setStyle(st);
        text->setHeight(getHeight());
        text->setWidth(300);
        text->setX(getWidth() / 2 - 150);

        text->addTween(TweenText("The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. The quick brown fox jumps over the lazy dog. 1234567890. "),
                       8000, -1, true);
    }
};