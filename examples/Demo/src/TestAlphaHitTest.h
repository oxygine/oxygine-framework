#pragma once
#include "test.h"

#undef OUT

class TestAlphaHitTest : public Test
{
public:
    spSprite sprite;
    spTextField txt;

    TestAlphaHitTest()
    {
        /*
        'flower' resource has hit_test option enabled. check res.xml
        <set hit_test = "true" / >
        <image file = "flower.png" / >

        'hit_test' enables access to alpha channel
        */

        sprite = new Sprite;
        sprite->setResAnim(resources.getResAnim("flower"));
        sprite->attachTo(_content);
        sprite->setAnchor(0.5f, 0.5f);
        sprite->setPosition(_content->getSize() / 2);

        sprite->addEventListener(TouchEvent::OVER, CLOSURE(this, &TestAlphaHitTest::onEvent));
        sprite->addEventListener(TouchEvent::OUTX, CLOSURE(this, &TestAlphaHitTest::onEvent));
        sprite->setTouchChildrenEnabled(false);

        txt = new TextField;
        txt->attachTo(_content);
        txt->setVAlign(TextStyle::VALIGN_BOTTOM);
        txt->setHAlign(TextStyle::HALIGN_MIDDLE);
        txt->setX(getWidth() / 2);
        txt->setY(getHeight() - 10);
        txt->setText("Move mouse over Sprite");
    }

    void onEvent(Event* ev)
    {
        TouchEvent* te = safeCast<TouchEvent*>(ev);
        if (ev->type == TouchEvent::OVER)
        {
            txt->setText("Mouse is OVER");
            sprite->setColor(Color::Green);
        }
        else
        {
            txt->setText("Mouse is OUT");
            sprite->setColor(Color::White);
        }

        spSprite sp = new Sprite;
        sp->setResAnim(resources.getResAnim("snow"));
        sp->setAnchor(0.5f, 0.5f);
        sp->setScale(0.5f);
        sp->setPosition(te->localPosition);
        sp->attachTo(sprite);
    }
};