#pragma once
#include "test.h"
#include "ClipRectActor.h"
#include "PointerState.h"
#include "SlidingActor.h"

class TestComplexDrag: public Test
{
public:
	TestComplexDrag()
	{
		spSprite innerContent = new Sprite;
		innerContent->setAnimFrame(resources.getResAnim("slide"));

		float x = 0;
		for (int i = 0; i < 5; ++i)
		{
			spButton button = new Button;			
			button->setResAnim(resourcesUI.getResAnim("button"));

			Vector2 pos(x, 100);
			pos.y = 0;
			button->setPosition(pos);
			button->attachTo(innerContent);
			button->setName("button");
			button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &TestComplexDrag::testClick));
			x += 200;
		}

		spSlidingActor frame = new SlidingActor();
		frame->setSize(Vector2(400, innerContent->getHeight()));
		frame->setContent(innerContent);
		frame->setPosition(getWidth()/2 - frame->getWidth()/2, 50.0f);
		frame->attachTo(this->content);

		spColorRectSprite innerContent2 = new ColorRectSprite();
		innerContent2->setSize(800, 200);
		innerContent2->setColor(Color(128,128,128,255));
		for (int i = 0; i < 8; i++)
		{
			spColorRectSprite c = new ColorRectSprite;
			c->setSize(100, 100);
			c->setX(i * 100.0f);
			c->setY((i % 2) * 100.0f);
			c->attachTo(innerContent2);

		}

		frame = new SlidingActor();
		frame->setSnapPageSize(Vector2(100, 100));
		frame->setSize(Vector2(400, 100));
		frame->setContent(innerContent2);
		frame->setPosition(getWidth()/2 - frame->getWidth()/2, 400.0f);
		frame->attachTo(this->content);
	}

	void testClick(Event *event)
	{
		showPopup("clicked");
	}
};
