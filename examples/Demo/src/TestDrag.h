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
			sprite->setAnimFrame(resources.getResAnim("batterfly"));
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

		actor->addTween(createTween(Sprite::TweenColor(Color(255,0,0,255)), 300, -1, true));
	}

	void onMouseUp(Event *event)
	{
		spSprite actor = safeSpCast<Sprite>(event->currentTarget);
		actor->removeTweens();		
		actor->setColor(Color(255,255,255,255));
	}
};
