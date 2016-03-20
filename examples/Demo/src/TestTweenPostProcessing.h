#pragma once
#include "test.h"
#include "TweenAlphaFade.h"
#undef OUT


class TestTweenPostProcessing : public Test
{
public:
    bool _fullscreen;
    bool _singleRender;
    int _type;

    spActor _test;
    TestTweenPostProcessing()
    {
        spSprite sprite = new Sprite;
        sprite->setResAnim(resources.getResAnim("t2p"));
        sprite->attachTo(content);

        sprite->setPosition(getStage()->getSize() / 2);

        spSprite anim = new Sprite;
        anim->attachTo(sprite);
        anim->addTween(Actor::TweenScale(3), 4000, -1, true);
        anim->setPosition(sprite->getSize() / 2);
        anim->setAnchor(0.5f, 0.5f);
        anim->setResAnim(resources.getResAnim("anim"));
        anim->addTween(TweenAnim(resources.getResAnim("anim")), 1500, -1);

        sprite->addTween(Actor::TweenRotationDegrees(360), 30000, -1);
        sprite->setAnchor(0.5f, 0.5f);


        _test = sprite;


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
            toggle("render: dynamic", 0),
            toggle("render: single", 1),
        };

        addToggle("mode.rn", t2, 2);


        toggle t3[] =
        {
            toggle("tween: Outline", 1),
            toggle("tween: AlphaFade", 0),
        };

        addToggle("type", t3, 2);

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
        else
            t = _test->addTween(TweenAlphaFade(true, opt), 5000, -1, true, 0, Tween::ease_inOutCubic);

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