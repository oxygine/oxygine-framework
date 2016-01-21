#pragma once
#include "test.h"
class TestEdges : public Test
{
public:
    TestEdges()
    {

        spSprite parent = new Sprite;

        for (int y = 0; y < 3; ++y)
            for (int x = 0; x < 3; ++x)
            {
                //spSprite sp = new ColorRectSprite;
                spSprite sp = new Sprite;
                sp->setResAnim(resources.getResAnim("edges"));

                sp->setSize(100.0f, 100.0f);
                sp->setPosition(x * 100.0f, y * 100.0f);
                sp->attachTo(parent);
            }

        content->addChild(parent);
        parent->setPosition(100.0f, 100.0f);
        parent->setAnchor(0.5f, 0.5f);
        parent->setSize(300.0f, 300.0f);
        parent->addTween(Actor::TweenPosition(1000.0f, 1000.0f), 30000, -1);
        parent->addTween(Actor::TweenRotationDegrees(360.0f), 10000, -1);
        parent->addTween(Actor::TweenScale(1.5f), 3000, -1, true);
    }
};
