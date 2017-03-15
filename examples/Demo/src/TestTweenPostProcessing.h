#pragma once
#include "test.h"

#undef OUT


class TestTweenPostProcessing : public Test
{
public:
    bool _fullscreen;
    bool _singleRender;
    int _type;

    spActor _test;

    spSprite createAnim(bool tween = true)
    {
        spSprite anim = new Sprite;
        anim->setAnchor(0.5f, 0.5f);
        anim->setResAnim(resources.getResAnim("anim"));

        anim->addTween(TweenAnim(resources.getResAnim("anim")), 1000, -1);
        if (tween)
            anim->addTween(Actor::TweenScale(3), 4000, -1, true);
        return anim;
    }

    TestTweenPostProcessing()
    {
        spSprite sprite = new Sprite;
        sprite->setResAnim(resources.getResAnim("t2p"));
        sprite->attachTo(content);
        sprite->setPosition(getStage()->getSize() / 2);
        sprite->addTween(Actor::TweenRotationDegrees(360), 30000, -1);
        sprite->setAnchor(0.5f, 0.5f);
        _test = sprite;


        spSprite anim;

        anim = createAnim(false);
        anim->addTween(Actor::TweenRotation(MATH_PI * 2), 15000, -1, true);
        anim->addTween(Actor::TweenX(sprite->getWidth()), 10000, -1, true);
        anim->addTween(Actor::TweenY(sprite->getHeight()), 12000, -1, true);
        anim->attachTo(sprite);


        anim = createAnim();
        anim->setPosition(sprite->getSize() / 2);
        anim->attachTo(sprite);

        anim = createAnim(false);
        anim->setPosition(0, sprite->getHeight() / 2);
        anim->attachTo(sprite);

        anim = createAnim(false);
        anim->setPosition(sprite->getWidth(), sprite->getHeight() / 2);
        anim->attachTo(sprite);


        _fullscreen = false;
        _singleRender = false;
        _type = 0;

        toggle t[] =
        {
            toggle("bounds: fullscreen", 0),
            toggle("bounds: actor", 1),
        };

        addToggle("mode.fs", t, 2);


        toggle t2[] =
        {
            toggle("render: dynamic", 1),
            toggle("render: static", 0),
        };

        addToggle("mode.rn", t2, 2);


        toggle t3[] =
        {
            toggle("tween: Outline", 1),
            toggle("tween: AlphaFade", 0),
            //toggle("tween: Glow", 2),
        };

        addToggle("type", t3, 2);

        //_type = 2;
        addTween();
    }

    void addTween()
    {
        _test->removeTweensByName("pp");

        PostProcessOptions opt;
        opt.fullscreen(_fullscreen);
        opt.singleRender(_singleRender);

        spTween t;
        if (_type == 0)
            t = _test->addTween(TweenOutline(Color::YellowGreen, opt), 5000, -1, true, 0, Tween::ease_inOutCubic);
        if (_type == 1)
            t = _test->addTween(TweenAlphaFade(true, opt), 5000, -1, true, 0, Tween::ease_inOutCubic);
        if (_type == 2)
            t = _test->addTween(TweenGlow(Color::YellowGreen, opt), 5000, -1, true, 0, Tween::ease_inOutCubic);


        t->setName("pp");
    }

    void toggleClicked(string id, const toggle* data)
    {
        if (id == "mode.fs")
        {
            _fullscreen = data->value ? true : false;
        }

        if (id == "mode.rn")
        {
            _singleRender = data->value ? true : false;
        }

        if (id == "type")
        {
            _type = data->value;
        }

        addTween();
    }

    void clicked(string id)
    {

        //   _test->addTween(TweenFade(), 1500);
    }
};