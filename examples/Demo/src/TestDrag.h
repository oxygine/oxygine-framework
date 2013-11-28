#pragma once
#include "test.h"
#include "Draggable.h"
class DraggableSprite: public Sprite
{
public:
	DraggableSprite()
	{
		drag.init(this);
	}

	Draggable drag;
};

class DragTest: public Test
{
public:
	DragTest()
	{
		for (int i = 0; i < 3; ++i)
		{
			spSprite sprite = new DraggableSprite;			
			Vector2 pos(100.0f * (i + 1), 150.0f);
			sprite->setPosition(pos);
			sprite->setResAnim(resources.getResAnim("batterfly"));
			sprite->attachTo(content);

			sprite->setRotation(scalar::randFloat(0, (float)MATH_PI * 2));
			sprite->setScale(scalar::randFloat(1.0f, 2.0f));

			/*
			for (int n = 0; n < 3; ++n)
			{
				DraggableSprite *child = new DraggableSprite;
				float scale = 0.5f;
				child->drag.setDragBounds(Rect(0,0, sprite->getWidth(), sprite->getHeight()));
				child->setScale(scale);
				child->setAnimFrame(resources.getResAnim("batterfly2"));
				child->attachTo(sprite);
				child->setX(n * 30.0f);
			}
			*/
		}
	}

	void onMouseDown(Event *event)
	{
		spActor actor = safeSpCast<Actor>(event->currentTarget);

		int new_priority = actor->getParent()->getLastChild()->getPriority() + 1;
		actor->setPriority(new_priority);

		actor->addTween(Sprite::TweenColor(Color(255,0,0,255)), 300, -1, true);
	}

	void onMouseUp(Event *event)
	{
		spSprite actor = safeSpCast<Sprite>(event->currentTarget);
		actor->removeTweens();		
		actor->setColor(Color::White);
	}
};


class Drag2Test: public Test
{
public:
	spSprite basket;
	spSprite ball;
	spSprite dragging;

	timeMS timeLeft;
	const PointerState *touchedBy;
	Draggable drag;
	
	Drag2Test():touchedBy(0), timeLeft(0)
	{
		basket = initActor(new Sprite,
			arg_name = "basket",
			arg_resAnim = resources.getResAnim("batterfly"),
			arg_attachTo= content,
			arg_x = content->getWidth() * 3 / 4,
			arg_y = content->getHeight() / 2);

		ball = initActor(new Sprite,
			arg_name = "ball",
			arg_resAnim = resources.getResAnim("batterfly"),
			arg_attachTo= content,
			arg_x = content->getWidth() * 1 / 4,
			arg_y = content->getHeight() / 2);

		ball->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this,  &Drag2Test::ballTouchDown));
		ball->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this,  &Drag2Test::ballTouchUp));
		content->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Drag2Test::touchUp));
	}

	
	void ballTouchUp(Event *event)
	{
		touchedBy = 0;
	}

	void touchUp(Event *event)
	{
		if (!dragging)
			return;
		if (event->currentTarget.get() != content)
			return;

		dragging->setColor(Color::White);
		spTween t;
		if (event->target == basket)
			t = dragging->addTween(Actor::TweenPosition(basket->getPosition()), 500);
		else
			t = dragging->addTween(Actor::TweenPosition(ball->getPosition()), 500);
		t->setDetachActor(true);
		dragging = 0;
	}


	void ballTouchDown(Event* ev)
	{
		TouchEvent *te = safeCast<TouchEvent*>(ev);
		touchedBy = te->getPointer();
		timeLeft = 500;		
	}

	void doUpdate(const UpdateState &us)
	{
		if (!timeLeft)
			return;
		if (!touchedBy)
			return;

		timeLeft -= us.dt;
		if (timeLeft <= 0)
		{
			timeLeft = 0;
			dragging = initActor(ball->clone(),
				arg_name = "dragging",
				arg_attachTo = ball->getParent(),
				arg_color = Color(0xff0000ff),
				arg_input = false);
			drag.start(touchedBy, dragging.get(), dragging->getSize()/2);
		}
	}
private:
};