#include "OptionsMenu.h"
#include <sstream>
#include "Options.h"
#include "shared.h"
#include "RootActor.h"
#include "blocking.h"

spOptionsMenu OptionsMenu::instance;

void OptionsApply()
{
	int volume = 0;
	volume = Options::instance.getValue("music").as_int();	

	volume = Options::instance.getValue("sounds").as_int();	
}

OptionsMenu::OptionsMenu()
{
	_timefadeIn = 500;
	_timeFadeOut = 500;

	EventCallback cb = CLOSURE(this, &OptionsMenu::clickedButton);
	_music = new ButtonWithText;
	_sounds = new ButtonWithText;
	_quality = new ButtonWithText;
	_back = new ButtonWithText;

	Vector2 pos(virtualSize.x/2.0f, 50);
	_music->init(getString("options", "music_on"), cb, pos, "music");
	pos.y += 70;
	_sounds->init(getString("options", "sounds_on"), cb, pos, "sounds");
	pos.y += 70;
	_quality->init(getString("options", "high_quality_on"), cb, pos, "high_quality");
	pos.y += 70;
	_back->init(getString("options", "back"), cb, pos, "back");


	_actor->addChild(_music);
	_actor->addChild(_sounds);
	_actor->addChild(_quality);
	_actor->addChild(_back);

	int volume = 0;
	volume = Options::instance.getValue("sounds").as_int();
	if (!volume)
		switchButton(_sounds);

	volume = Options::instance.getValue("music").as_int();
	if (!volume)
		switchButton(_music);

	bool high = Options::instance.getValue("high_quality").as_bool();
	if (!high)
		switchButton(_quality);

	OptionsApply();

	_actor->setSize(getRoot()->getSize());
	_actor->setAnchor(Vector2(0.5f, 0.5f));
	_actor->setPosition(getRoot()->getSize()/2);
}

OptionsMenu::~OptionsMenu()
{

}


void OptionsMenu::doShowing()
{
	_actor->setAlpha(0);
	_actor->setScale(1.0f);
	_actor->setRotation(0);
	spTween t = createTween(Actor::TweenAlpha(255), _timefadeIn);
	spTween rot = createTween(Actor::TweenRotation(MATH_PI*2), _timefadeIn);
	_actor->addTween(t);
	_actor->addTween(rot);

	blocking::waitTween(t);
}

void OptionsMenu::doHiding()
{
	spTween t = createTween(Actor::TweenAlpha(0), _timeFadeOut);
	spTween sc = createTween(Actor::TweenScale(Vector2(10, 10)), _timeFadeOut);
	_actor->addTween(t);
	_actor->addTween(sc);
	blocking::waitTween(t);
}

void OptionsMenu::clickedButton(Event *event)
{
	spActor btn = safeSpCast<Actor>(event->currentTarget);
	const string &name = btn->getName();
	if (name == "back")
		generateActionByEvent(event);
	if (name == "music")
	{
		switchButton(_music);

		int volume = _music->enabled ? 100 : 0;
		Options::instance.getValue("music").set_value(volume);
		
	}

	if (name == "sounds")
	{
		switchButton(_sounds);

		int volume = _sounds->enabled ? 100 : 0;
		Options::instance.getValue("sounds").set_value(volume);		
	}

	if (name == "high_quality")
	{
		switchButton(_quality);
		Options::instance.getValue("high_quality").set_value(_quality->enabled);
	}

	Options::instance.save();

	OptionsApply();	
}

void OptionsMenu::doLoop()
{
	while(1)
	{
		string action = waitAction();

		if (action == "back")
			break;
	}
}

void OptionsMenu::switchButton(spButtonWithText btn)
{
	btn->enabled = !btn->enabled;
	string s = btn->getName() + (btn->enabled ? "_on" : "_off");
	btn->setText(getString("options", s.c_str()));
}
