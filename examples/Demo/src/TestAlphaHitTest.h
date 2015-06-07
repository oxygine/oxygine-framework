#pragma once
#include "test.h"
#include "initActor.h"

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

        sprite = initActor(new Sprite,
                           arg_resAnim = resources.getResAnim("flower"),
                           arg_attachTo = content,
                           arg_anchor = Vector2(0.5f, 0.5f),
                           arg_x = content->getWidth() / 2,
                           arg_y = content->getHeight() / 2);

        sprite->addEventListener(TouchEvent::OVER, CLOSURE(this, &TestAlphaHitTest::onEvent));
        sprite->addEventListener(TouchEvent::OUT, CLOSURE(this, &TestAlphaHitTest::onEvent));

        txt = new TextField;
        txt->attachTo(content);
        txt->setVAlign(TextStyle::VALIGN_BOTTOM);
        txt->setHAlign(TextStyle::HALIGN_MIDDLE);
        txt->setX(getWidth() / 2);
        txt->setY(getHeight() - 10);
        txt->setText("Move mouse over Sprite");
    }

    void onEvent(Event* ev)
    {
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
    }
};