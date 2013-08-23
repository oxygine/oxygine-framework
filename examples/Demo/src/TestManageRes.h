#pragma once
#include "test.h"
#include "Multithreading.h"
class ManageResTest: public Test
{
public:
	bool loaded;

	ManageResTest()
	{
		loaded = true;
		addButton("switch", "Load/Unload resources");
		addButton("mt", "Multithreading loading");
		addButton("mt_slow", "MT loading (slow demo)");


		for (int i = 0; i < resources.getCount(); ++i)
		{
			ResAnim *ra = dynamic_cast<ResAnim *>(resources.get(i));
			if (!ra)
				continue;
			if (ra->getName().find("_") != string::npos)
				continue;
			spSprite sprite = new Sprite;
			sprite->setAnimFrame(ra);
			sprite->addTween(createTween(TweenAnim(ra), 500, -1));
			sprite->setPosition((int)scalar::randFloat(50.0f, getWidth() - 100.0f), (int)scalar::randFloat(50.0f, getHeight() - 100.0f));
			sprite->attachTo(content);	

			if (ra->isName("bg"))
			{
				sprite->setPosition(0,0);
				sprite->setScale(getWidth()/(float)sprite->getWidth());
				sprite->setPriority(-1);
			}
			else
			{
				//sprite->addEventHandler(new DragHandler);
			}
		}

		spTextActor text = new TextActor;
		text->attachTo(content);

		text->setSize(300, 200);
		text->setPosition(140.0f, (float)getHeight() - text->getHeight());

		TextStyle st;
		st.font = resourcesUI.getResFont("big")->getFont();
		st.vAlign = TextStyle::VALIGN_TOP;
		st.color = Color(0xffffffff);
		st.multiline = true;
		text->setStyle(st);
		text->setText("The quick brown fox jumps over the lazy dog. 1234567890");
		//text->addEventHandler(new DragHandler);
	}

	void _loaded(Event *event)
	{
		showPopup("Loaded!");
		ui->getChild("loading")->addTween(createTween(Sprite::TweenAlpha(0), 400))->setDetachActor(true);
	}

	void clicked(string id)
	{
		if (id == "switch")
		{
			if (loaded)
				resources.unload();
			else
				resources.load();

			loaded = !loaded;
		}

		if (id == "mt" || id == "mt_slow")
		{
			resources.unload();

			spSprite sp = new Sprite;
			sp->setName("loading");
			sp->setAnimFrame(resourcesUI.getResAnim("loading"));
			sp->attachTo(ui);
			sp->setAnchor(Vector2(0.5f, 0.5f));
			sp->setPosition(getSize() - sp->getSize()/4);
			sp->setScale(0.5f);
			sp->addTween(createTween(Actor::TweenRotation(-(float)MATH_PI * 2), 1500, -1));

			spThreadLoading l = new ThreadLoading;
			l->addEventListener(ThreadLoading::COMPLETE, CLOSURE(this, &ManageResTest::_loaded));
			l->add(&resources);
			if (id == "mt_slow")
				l->setUpdateSize(128);
			l->start(RootActor::instance);
		}
	}
};
