#pragma once
#include "test.h"
#include "InitActor.h"

class TestMask: public Test
{
public:
	timeMS _lastSnow;

	spMaskedSprite _masked;
	spSprite _mask;

	TestMask():_lastSnow(0)
	{		
		_mask = initActor(new Sprite, 
			arg_scale = 3,
			arg_alpha = 64,
			arg_x = 100,
			arg_y = 50,
			arg_anchor = Vector2(0.5f, 0.5f),
			arg_resAnim = resources.getResAnim("logo2"),
			arg_attachTo = content);

		_mask->addTween(Actor::TweenRotation(MATH_PI * 2), 15000, -1, true);
		_mask->addTween(Actor::TweenX(content->getWidth() - 100), 10000, -1, true);
		_mask->addTween(Actor::TweenY(content->getHeight() - 50), 12000, -1, true);

		_masked = initActor(new MaskedSprite,
			arg_attachTo = content);

		_masked->setMask(_mask);


		TextStyle style;
		style.font = resourcesUI.getResFont("main")->getFont();
		style.color = Color(0, 255, 50, 255);
		style.vAlign = TextStyle::VALIGN_MIDDLE;
		style.hAlign = TextStyle::HALIGN_CENTER;
		style.multiline = true;

		spTextActor text = initActor(new TextActor(), 
			arg_attachTo = _masked,
			arg_scale = 2,
			arg_pos = content->getSize()/2,
			arg_text = "Oxygine. Masked text",
			arg_style = style);		



		addButton("show_mask", "hide mask");
		addButton("change_mask1", "change mask");
		addButton("disable_mask", "disable mask");
		addButton("pause", "pause");
	}

	void clicked(string id)
	{
		if (id == "pause")
		{
			spClock clock = getRoot()->getClock();
			if (!clock->getPauseCounter())
				clock->pause();
			else
				clock->resume();
		}
		if (id == "show_mask")
		{
			_mask->setVisible(!_mask->getVisible());
			if (_mask->getVisible())
				updateText("show_mask", "hide mask");
			else
				updateText("show_mask", "show mask");
		}

		if (id == "change_mask1") 
		{			
			_mask->addTween(TweenAnim(resources.getResAnim("anim")), 600, -1, false);
		}


		if (id == "disable_mask") 
		{
			if (_masked->getMask())
			{
				updateText("disable_mask", "enable mask");
				_masked->setMask(0);
			}
			else
			{
				updateText("disable_mask", "disable mask");
				_masked->setMask(_mask);
			}
		}
	}

	void doUpdate(const UpdateState &us)
	{
		if (_lastSnow  + 40 < us.time)
		{
			_lastSnow = us.time;
			spSprite snow = initActor(new Sprite,
				arg_anchor = Vector2(0.5f, 0.5f),
				arg_resAnim = resources.getResAnim("snow"),
				arg_attachTo = _masked,
				arg_rotation = scalar::randFloat(0, MATH_PI * 2),
				arg_y = -50,
				arg_scale = scalar::randFloat(0.4f, 1.0f),
				arg_x = scalar::randFloat(0, content->getWidth()));

			snow->addTween(Actor::TweenY(content->getHeight() + 50), 6000)->setDetachActor(true);
			snow->addTween(Actor::TweenRotation(scalar::randFloat(0, MATH_PI * 2)), 10000);
		}
	}
};
