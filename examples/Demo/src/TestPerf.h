#pragma once
#include "test.h"

class PerfTest: public Test
{
public:
	spButton button;
	int count;
	bool _tweenAnim;
	bool _tweenRot;
	bool _tweenScale;

	PerfTest()
	{
		_tweenAnim = false;
		_tweenRot = false;
		_tweenScale = false;
		count = 0;
		addButton("add", "add 250");
		addButton("animate", "animate");
		addButton("scale0.01", "scale=0.01");
		addButton("scale0.2", "scale=0.2");
		addButton("scale0.5", "scale=0.5");
		addButton("driver", "driver=null");

		content->setInputEnabled(false);
		content->setInputChildrenEnabled(false);
	}

	void clicked(string id)
	{
		if (id == "driver")
		{
			if (content->driver)
			{
				delete content->driver;
				content->driver = 0;
			}
			else
				content->driver = new VideoDriverNull;

			updateText(id, !content->driver ? "driver=null" : "driver=default");
		}

		if (id == "add")
		{
			int a = 250;
			count += a;
			for (int i = 0; i < a; ++i)
			{
				spSprite sprite = new Sprite;
				sprite->setAnimFrame(resources.getResAnim("anim"));
				sprite->setAnchor(Vector2(0.5f, 0.5f));
				sprite->setScale(0.05f);
				sprite->setPosition(scalar::randFloat(0, (float)getWidth()), scalar::randFloat(0, (float)getHeight()));
				content->addChild(sprite);
			}

			char str[255];
			sprintf(str, "add 250 (%d)", count);			
			updateText(id, str);
		}
		if (id == "scale0.01")
		{
			spActor child = content->getFirstChild();
			while(child)
			{
				child->setScale(Vector2(1,1) * 0.01f);
				child = child->getNextSibling();
			}
		}
		if (id == "scale0.2")
		{
			spActor child = content->getFirstChild();
			while(child)
			{
				child->setScale(Vector2(1,1) * 0.2f);
				child = child->getNextSibling();
			}
		}

		if (id == "scale0.5")
		{
			spActor child = content->getFirstChild();
			while(child)
			{
				child->setScale(Vector2(1,1) * 0.5f);
				child = child->getNextSibling();
			}
		}

		if (id == "animate")
		{
			spActor child = content->getFirstChild();
			while(child)
			{
				spTween t = 0;
				if (!_tweenAnim)
					t = createTween(TweenAnim(resources.getResAnim("anim")), 500, -1);
				else
				if (!_tweenRot)
					t = createTween(Actor::TweenRotation((float)MATH_PI * 2.0f), 3000, -1);
				else
				if (!_tweenScale)
					t = createTween(Actor::TweenScale(Vector2(0.2f, 0.2f)), 3000, -1, true);


				if (t)
					child->addTween(t);
				child = child->getNextSibling();
			}

			if (!_tweenAnim)
				_tweenAnim = true;
			else
			{
				if (!_tweenRot)
					_tweenRot = true;
				else
					if (!_tweenScale)
					{
						updateText(id, "");
						_tweenScale = true;
					}
			}
		}
	}

};