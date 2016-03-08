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

        sprite->setPosition(200.8f, 150);
        sprite->addEventListener(TouchEvent::CLICK,
                                 CLOSURE(this, &TestTweenAlphaFade::onClick));

        spSprite anim = new Sprite;
        //anim->attachTo(sprite);
        //anim->setScale(1.0f / 1.5f);
        anim->setPosition(sprite->getSize() / 2);
        anim->setAnchor(0.5f, 0.5f);
        anim->setScale(10);
        anim->setResAnim(resources.getResAnim("anim"));
        anim->addTween(TweenAnim(resources.getResAnim("anim")), 1500, -1);


        _test = sprite;
    }

    void onClick(Event*)
    {
        _test->addTween(TweenAlphaFade(false, 0), 5500, 1, true);
    }

    void clicked(string id)
    {
        //   _test->addTween(TweenFade(), 1500);
    }
};