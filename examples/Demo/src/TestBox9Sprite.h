#pragma once
#include "test.h"

class TestBox9Sprite: public Test
{
public:
    spBox9Sprite _sprite;
    TestBox9Sprite()
    {
        ResAnim* rs = resources.getResAnim("box9a");
        spSprite original = new Sprite;
        original->setPosition(10, 70);
        original->setResAnim(rs);
        addChild(original);



        spBox9Sprite sprite;

        sprite = new Box9Sprite;
        sprite->setPosition(270, 50);
        sprite->setResAnim(rs);
        sprite->setSize(300, 250);
        sprite->setVerticalMode(Box9Sprite::STRETCHING);
        sprite->setHorizontalMode(Box9Sprite::STRETCHING);


        sprite->addTween(Actor::TweenWidth(getWidth() - sprite->getX() - 190), 5000, -1, true);
        sprite->addTween(Actor::TweenHeight(getHeight() - sprite->getY() - 10), 5000, -1, true, 2500);


        //You could also define guides from xml
        //<image file="box9.png" guideX1="50" guideX2="210" guideY1="50" guideY2="125" />
        sprite->setGuides(40, 160, 40, 160);
        addChild(sprite);

        _sprite = sprite;

        Test::toggle tg[] =
        {
            Test::toggle("switch to TILING_FULL", Box9Sprite::TILING_FULL),
            Test::toggle("switch to TILING", Box9Sprite::TILING),
            Test::toggle("switch to STRETCHING", Box9Sprite::STRETCHING),
        };
        addToggle("ver. mode", tg, 3);
        addToggle("hor. mode", tg, 3);
    }

    void toggleClicked(string id, const toggle* t)
    {
        if (id == "ver. mode")
            _sprite->setVerticalMode(Box9Sprite::StretchMode(t->value));
        if (id == "hor. mode")
            _sprite->setHorizontalMode(Box9Sprite::StretchMode(t->value));
    }
};
