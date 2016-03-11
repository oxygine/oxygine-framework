#pragma once
#include "test.h"
#include "TweenAlphaFade.h"
#undef OUT


class TestTweenAlphaFade : public Test
{
public:
    spActor _test;
    TestTweenAlphaFade()
    {
        spSprite sprite = new Sprite;
        sprite->setResAnim(resources.getResAnim("t2p"));
        sprite->attachTo(content);

        sprite->setPosition(getStage()->getSize() / 2);
        sprite->addEventListener(TouchEvent::CLICK,
                                 CLOSURE(this, &TestTweenAlphaFade::onClick));

        spSprite anim = new Sprite;
        anim->attachTo(sprite);
        anim->addTween(Actor::TweenScale(3), 4000, -1, true);
        anim->setPosition(sprite->getSize() / 2);
        anim->setAnchor(0.5f, 0.5f);
        anim->setScale(10);
        anim->setResAnim(resources.getResAnim("anim"));
        anim->addTween(TweenAnim(resources.getResAnim("anim")), 1500, -1);

        sprite->addTween(Actor::TweenRotationDegrees(360), 10000, -1);
        sprite->setAnchor(0.5f, 0.5f);
        _test = sprite;
    }

    void onClick(Event*)
    {
        _test->addTween(TweenAlphaFade(false, TweenAlphaFade::opt_singleR2T), 5500, 1, true);
    }

    void clicked(string id)
    {
        //   _test->addTween(TweenFade(), 1500);
    }
};