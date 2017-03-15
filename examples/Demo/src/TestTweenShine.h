#pragma once
#include "test.h"

class TweenShine: public Tween
{
public:

    TweenShine()
    {
    }

    void _loopDone(Actor& actor, const UpdateState& us) override
    {
        for (int i = 0; i < 5; ++i)
        {
            Vector2 pos;
            pos.x = scalar::randFloat(0, actor.getWidth());
            pos.y = scalar::randFloat(0, actor.getHeight());

            if (!actor.isOn(pos))
                continue;

            spSprite sprite = new Sprite;
            sprite->setResAnim(resources.getResAnim("snow"));
            sprite->setAnchor(0.5f, 0.5f);
            sprite->setPosition(pos);
            //sprite->setAlpha(0);
            sprite->setScale(0.5f);
            sprite->addTween(Actor::TweenScale(1.5f), 500, 1, false, 0, Tween::ease_inBack);
            sprite->addTween(Actor::TweenAlpha(0), TweenOptions(500).delay(800).detach(true));
            actor.addChild(sprite);
            //break;
        }
    }

    static spTween create(int duration, int loops)
    {
        spTween t = new TweenShine;
        t->setDuration(duration);
        t->setLoops(loops);
        return t;
    }
};


class TestTweenShine: public Test
{
public:
    spSprite sprite;
    TestTweenShine()
    {
        sprite = new Sprite;
        sprite->setResAnim(resources.getResAnim("flower"));
        sprite->attachTo(content);
        //sprite->setAnchor(0.5f, 0.5f);
        sprite->setPosition(content->getSize() / 2 - sprite->getSize() / 2);

        sprite->addTween(TweenShine::create(10, 300));

        addButton("shine", "shine");
    }

    void clicked(string id)
    {
        sprite->addTween(TweenShine::create(10, 300));
    }
};