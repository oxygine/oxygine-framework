#pragma once
#include "test.h"
#include "ColorRectSprite.h"
#include <typeinfo>

template <class T>
class dummy
{

};

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
		addButton("RelTweenRotation", "Add Relative TweenRotation");
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
		_sprite->setAnchor(Vector2(0.5f, 0.5f));
		_sprite->setAnimFrame(resources.getResAnim("anim"));
		_sprite->setPosition(getWidth()/2.0f, (float)getHeight() - _sprite->getHeight());		

		updateEase();
	}
	
	spActor createEaseTest(Tween::EASE ease)
	{
		spColorRectSprite bg = new ColorRectSprite();
		bg->setSize(Vector2(230, 230));
		bg->setColor(Color(128, 128, 128, 255));

		spColorRectSprite parent = new ColorRectSprite();
		parent->setSize(Vector2(200, 200));
		parent->setColor(Color(230, 230, 230, 255));

		float f = 0;
		while (f <= 1.001f)
		{
			spColorRectSprite cr = new ColorRectSprite();
			cr->setSize(Point(2, 2));
			cr->setColor(Color(255, 0, 0, 255));
			parent->addChild(cr);

			float x = f * parent->getWidth();
			float y = (1.0f - Tween::calcEase(ease, f)) * parent->getHeight();

			cr->setPosition(Vector2(x - 1, y - 1));

			f += 0.005f;
		}

		parent->setPosition(15, 15);
		bg->addChild(parent);

		return bg;
	}

	void updateEase()
	{
		if (_easeGraph)
			_easeGraph->detach();		

		_easeGraph = createEaseTest(_ease);
		_easeGraph->setPriority(-1);
		_easeGraph->setY(150);
		content->addChild(_easeGraph);

		updateText("ease", enum2string(_ease));
	}

	void tweenDone(Event *event)
	{
		showPopup("TweenDone");
	}

	void _addTween(spTween tween, bool rel = false)
	{
		tween->setEase(_ease);
		if (rel)
			_sprite->addTweenRelative(tween);
		else
			_sprite->addTween(tween);
		tween->setDoneCallback(CLOSURE(this, &TweensTest::tweenDone));
	}

	string enum2string(Tween::EASE e)
	{
		string str = "unknown";
		switch(_ease)
		{
        case Tween::ease_unknown:
            str = "ease: unknown";
            break;
		case Tween::ease_linear:
			str = "ease: linear";
			break;
		case Tween::ease_inExpo:
			str = "ease: inExpo";
			break;
		case Tween::ease_outExpo:
			str = "ease: outExpo";
			break;
		case Tween::ease_inSin:
			str = "ease: inSin";
			break;
		case Tween::ease_outSin:
			str = "ease: outSin";
			break;
		case Tween::ease_inCubic:
			str = "ease: inCubic";
			break;
		case Tween::ease_outCubic:
			str = "ease: outCubic";
			break;
		case Tween::ease_inOutBack:
			str = "ease: inOutBack";
			break;
		case Tween::ease_inBack:
			str = "ease: inBack";
			break;
		case Tween::ease_outBack:
			str = "ease: outBack";
			break;
		}
		return str;
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
			_addTween( createTween(TweenAnim(resources.getResAnim("anim")), dur/2, 1));
		}
		if (id == "TweenRotation")
		{
			_addTween( createTween(Actor::TweenRotation(_sprite->getRotation() + (float)MATH_PI * 2), dur, 1));
		}
		if (id == "RelTweenRotation")
		{
			_addTween( createTween(Actor::TweenRotation((float)MATH_PI / 2), dur/2, 1), true);
		}
		if (id == "TweenPosition")
		{
			_addTween( createTween(Actor::TweenPosition(_sprite->getPosition() + Vector2(0, -200)), dur, 1, true));
		}
		if (id == "TweenScale")
		{
			_addTween( createTween(Actor::TweenScale(2, 2), dur, 1, true));
		}
		if (id == "TweenColor")
		{
			_addTween( createTween(Sprite::TweenColor(Color(255,0,0,255)), dur, 1, true));
		}
		if (id == "TweenAlpha")
		{
			_addTween( createTween(Actor::TweenAlpha(0), dur, 1, true));
		}
		if (id == "TweenDummy")
		{
			_addTween( createTween(TweenDummy(), dur, 1, true));
		}

		if (id == "TweenDelay")
		{
			_addTween( createTween(Actor::TweenRotation(_sprite->getRotation() + (float)MATH_PI * 2), dur, 2, false, 1000));
		}

		if (id == "TweenQueue")
		{
			spTweenQueue queue = new TweenQueue();			
			queue->add(createTween(TweenAnim(resources.getResAnim("anim")), 500, 1));
			queue->add(createTween(Actor::TweenRotation(_sprite->getRotation() + (float)MATH_PI * 2.0f), 500, 1, false, 1000));
			queue->add(createTween(Actor::TweenScale(2), 500, 1, true));
			_addTween(queue);
		}

		if (id == "complete")
		{
			spTween t = _sprite->getFirstTween();
			while(t)
			{
				spTween next = t->getNextSibling();
				t->complete();//removes self from actor
				t = next;
			}
		}

	}
};
