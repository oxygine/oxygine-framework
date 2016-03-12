#pragma once
#include "test.h"
#include "TweenAlphaFade.h"
#undef OUT


class TestTweenGlow : public Test
{
public:
    spActor _test;
    TestTweenGlow()
    {
        spSprite sprite = new Sprite;
        sprite->setResAnim(resources.getResAnim("t2p"));
        sprite->attachTo(content);

        sprite->setPosition(getStage()->getSize() / 2);

        spSprite anim = new Sprite;
        anim->attachTo(sprite);
        anim->addTween(Actor::TweenScale(6), 4000, -1, true);
        anim->setPosition(sprite->getSize() / 2);
        anim->setAnchor(0.5f, 0.5f);
        anim->setScale(10);
        anim->setResAnim(resources.getResAnim("anim"));
        anim->addTween(TweenAnim(resources.getResAnim("anim")), 1500, -1);

        sprite->addTween(Actor::TweenRotationDegrees(360), 10000, -1);
        sprite->setAnchor(0.5f, 0.5f);

        sprite->addTween(TweenGlow(Color::Blue), 55000, 1, true);
        _test = sprite;
    }

    void clicked(string id)
    {
        //   _test->addTween(TweenFade(), 1500);
    }
};