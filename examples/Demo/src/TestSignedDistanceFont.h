#pragma once
#include "test.h"

class TestSignedDistanceFont : public Test
{
public:
    spTextField _txt;
    ResFontBM font;

    spTween t;
    TestSignedDistanceFont()
    {
        font.initSD("sdf/font.fnt", 8);
        font.load();


        /*how to make SD font
        based on article: https://habrahabr.ru/post/282191/
        1. install ImageMagick
        2. generate huge font with size 400pt, set padding 40x40x40x40
        3. convert font image to SD font image using command below:

        convert font.png -filter Jinc ( +clone -negate -morphology Distance Euclidean -level 50%,-50% ) -morphology Distance Euclidean -compose Plus -composite -level 43%,57% -resize 12.5% font.png

        font image will be downscaled 8 times from 4096 to 512

        declare your font in resources.xml:
        <sdfont font="font.fnt" downsample="8" />
        or load it directly from file as I did above
        */



        spTextField txt = new TextField;
        txt->attachTo(content);
        _txt = txt;

        TextStyle st;
        st.font = &font;
        st.vAlign = TextStyle::VALIGN_MIDDLE;
        st.hAlign = TextStyle::HALIGN_MIDDLE;
        st.color = Color::CornflowerBlue;

        st.multiline = true;

        st.outlineColor = Color::White;
        st.outline = 0.1f;

        txt->setStyle(st);
        txt->setColor(Color::CornflowerBlue);
        txt->setText("The quick brown fox jumps over the lazy dog. 1234567890.");
        txt->setPosition(getStage()->getSize() / 2);
        txt->setWidth(getStage()->getWidth() / 2);
        txt->setAnchor(0.5f, 0.5f);



        addButton("scale+", "scale+");
        addButton("scale-", "scale-");

        addButton("weight+", "weight+");
        addButton("weight-", "weight-");

        addButton("outline+", "outline+");
        addButton("outline-", "outline-");
    }

    ~TestSignedDistanceFont()
    {
    }

    void clicked(string id)
    {
        if (id == "outline+")
            _txt->setOutline(_txt->getOutline() + 0.01f);
        if (id == "outline-")
            _txt->setOutline(_txt->getOutline() - 0.01f);

        if (id == "scale+")
            _txt->addTween(TweenScale(_txt->getScale() * 1.5f), 300);
        if (id == "scale-")
            _txt->addTween(TweenScale(_txt->getScale() / 1.5f), 300);

        if (id == "weight+")
            _txt->setWeight(_txt->getWeight() - 0.01f);
        if (id == "weight-")
            _txt->setWeight(_txt->getWeight() + 0.01f);
    }

    void toggleClicked(string id, const toggle* data)
    {

    }
};

