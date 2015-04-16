#pragma once
#include "test.h"
#include "ColorRectSprite.h"
#include <typeinfo>

class TweensTest: public Test
{
public:
    spSprite _sprite;
    Tween::EASE _ease;
    spActor _easeGraph;

    TweensTest()
    {
        _ease = Tween::ease_linear;
        //_ease = Tween::ease_inOutBack;
        addButton("ease", "ease: Linear");
        addButton("TweenAnim", "Add TweenAnim");
        addButton("TweenRotation", "Add TweenRotation");
        addButton("TweenScale", "Add TweenScale");
        addButton("TweenPosition", "Add TweenPosition");
        addButton("TweenColor", "Add TweenColor");
        addButton("TweenAlpha", "Add TweenAlpha");
        addButton("TweenDummy", "Add TweenDummy");
        addButton("TweenQueue", "Add Queue of Tweens");
        addButton("TweenDelay", "Add Tween with Delay");
        addButton("complete", "Complete Tween");


        _sprite = new Sprite();
        _sprite->attachTo(content);
        _sprite->setAnchor(0.5f, 0.5f);
        _sprite->setResAnim(resources.getResAnim("anim"));
        _sprite->setPosition(getWidth() / 2.0f, getHeight() / 2);

        updateEase();
    }

    spActor createEaseTest(Tween::EASE ease)
    {
        spColorRectSprite bg = new ColorRectSprite();
        bg->setSize(330, 230);
        bg->setColor(Color::Gray);

        spColorRectSprite parent = new ColorRectSprite();
        parent->setSize(300, 200);
        parent->setColor(Color(230, 230, 230, 255));

        float f = 0;
        while (f <= 1.001f)
        {
            spColorRectSprite cr = new ColorRectSprite();
            cr->setSize(2, 2);
            cr->setColor(Color::Red);
            parent->addChild(cr);

            float x = f * parent->getWidth();
            float y = (1.0f - Tween::calcEase(ease, f)) * parent->getHeight();

            cr->setPosition(x - 1, y - 1);

            f += 0.0025f;
        }

        parent->setPosition(10, 15);
        bg->addChild(parent);

        return bg;
    }

    void updateEase()
    {
        if (_easeGraph)
            _easeGraph->detach();

        _easeGraph = createEaseTest(_ease);
        _easeGraph->setPriority(-1);
        _easeGraph->setY(getHeight() / 2 - _easeGraph->getHeight() / 2);
        content->addChild(_easeGraph);

        updateText("ease", "ease: " + ease2String(_ease));
    }

    void tweenDone(Event* event)
    {
        notify("TweenDone");
    }

    void _addTween(spTween tween)
    {
        tween->setEase(_ease);
        _sprite->addTween(tween);
        tween->addEventListener(TweenEvent::DONE, CLOSURE(this, &TweensTest::tweenDone));
    }

    void clicked(string id)
    {
        if (id == "ease")
        {
            _ease = (Tween::EASE)(_ease + 1);
            _ease = (Tween::EASE)(_ease % Tween::ease_count);
            if (_ease == Tween::ease_unknown)
                _ease = Tween::ease_linear;
            updateEase();
        }

        int dur = 2000;
        if (id == "TweenAnim")
        {
            _addTween(createTween(TweenAnim(resources.getResAnim("anim")), dur / 4, 10));
        }
        if (id == "TweenRotation")
        {
            _addTween(createTween(Actor::TweenRotation(_sprite->getRotation() + (float)MATH_PI * 2), dur, 1));
        }
        if (id == "TweenPosition")
        {
            _addTween(createTween(Actor::TweenPosition(_sprite->getPosition() + Vector2(0, -200)), dur, 1, true));
        }
        if (id == "TweenScale")
        {
            _addTween(createTween(Actor::TweenScale(2, 2), dur, 1, true));
        }
        if (id == "TweenColor")
        {
            _addTween(createTween(Sprite::TweenColor(Color(255, 0, 0, 255)), dur, 1, true));
        }
        if (id == "TweenAlpha")
        {
            _addTween(createTween(Actor::TweenAlpha(0), dur, 1, true));
        }
        if (id == "TweenDummy")
        {
            _addTween(createTween(TweenDummy(), dur, 1, true));
        }

        if (id == "TweenDelay")
        {
            _addTween(createTween(Actor::TweenRotation(_sprite->getRotation() + (float)MATH_PI * 2), dur, 2, false, 1000));
        }

        if (id == "TweenQueue")
        {
            spTweenQueue queue = new TweenQueue();
            queue->add(TweenAnim(resources.getResAnim("anim")), 500, 1);
            queue->add(Actor::TweenRotation(_sprite->getRotation() + (float)MATH_PI * 2.0f), 500, 1, false, 1000);
            queue->add(Actor::TweenScale(2), 500, 1, true);
            _addTween(queue);
        }

        if (id == "complete")
        {
            _sprite->removeTweens(true);
            /*
            spTween t = _sprite->getFirstTween();
            while(t)
            {
                spTween next = t->getNextSibling();
                t->complete();//removes self from actor
                t = next;
            }
            */
        }

    }
};
