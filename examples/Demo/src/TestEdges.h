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
                spSprite sp = new Sprite;
                sp->setResAnim(resources.getResAnim("edges"));
                sp->setSize(100.0f, 100.0f);
                sp->setPosition(x * 100.0f, y * 100.0f);
                sp->attachTo(parent);
            }

        content->addChild(parent);
        parent->setPosition(getStage()->getSize() / 2 - Vector2(200, 200));
        parent->setAnchor(0.5f, 0.5f);
        parent->setSize(300.0f, 300.0f);
        parent->addTween(Actor::TweenPosition(getStage()->getSize() / 2 + Vector2(200, 200)), 30000, -1, true);
        parent->addTween(Actor::TweenRotationDegrees(360.0f), 10000, -1);
        parent->addTween(Actor::TweenScale(1.5f), 3000, -1, true);
    }
};
