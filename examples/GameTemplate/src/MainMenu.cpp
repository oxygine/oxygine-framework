#include "MainMenu.h"
#include "RootActor.h"
#include "GameActor.h"
#include "Tweener.h"
#include "OptionsMenu.h"
#include "shared.h"


spMainMenu MainMenu::instance;

ButtonWithText::ButtonWithText():enabled(true)
{
	_text = new TextActor;
	addChild(_text);
	setAnchor(Vector2(0.5f, 0.5f));
}

ButtonWithText::~ButtonWithText()
{

}

void ButtonWithText::init(const string &text, EventCallback &cb, const Vector2 &pos, const string &name)
{
	setName(name);
	setPosition(pos);
	addEventListener(TouchEvent::CLICK, cb);
	setChildrenRelative(false);

	setResAnim(gameResources.getResAnim("button"));


	TextStyle style = basicStyle;	
	style.hAlign = TextStyle::HALIGN_CENTER;
	style.vAlign = TextStyle::VALIGN_MIDDLE;
	style.multiline = false;

	_text->setStyle(style);	
	setText(text);	
}

void ButtonWithText::setText(const string &str)
{
	_text->setText(str);
}

MainMenu::MainMenu()
{	
	spClock clock = new Clock();
	clock->setFixedStep(1000/50.0f);
	_actor->setClock(clock);
			
	_actor->setSize(getRoot()->getSize());


	spSprite bg = new Sprite();
	bg->setAnimFrame(gameResources.getResAnim("bg"));
	_actor->addChild(bg);
	


	EventCallback cb = CLOSURE(this, &MainMenu::clickedButton);

	int dy = 70;
	Vector2 pos(virtualSize.x/2.0f, 100);

	_play = new ButtonWithText;
	_play->init(getString("main_menu", "play"), cb, pos, "play");

	
	pos.y += dy;
	_options = new ButtonWithText;
	_options->init(getString("main_menu", "options"), cb, pos, "options");
	
	pos.y += dy;
	
	
	_menu = new Actor;
	_menu->addChild(_play);	
	_menu->addChild(_options);

	_actor->addChild(_menu);
}

MainMenu::~MainMenu()
{

}

void MainMenu::doLoop()
{
	while(1)
	{
		string action = waitAction();

		if (action == "play")
		{
			hiding();

			spActor parent = _actor->detach();

			spGameActor game = new GameActor();
			getRoot()->addChild(game->_actor);
								
			game->loop();

			game->_actor->detach();

			parent->addChild(this->_actor);

			showing();
		}
		if (action == "options")
		{				
			_menu->setInputEnabled(false);

			_menu->setAlpha(255);
			spTween t = _menu->addTween(Actor::TweenAlpha(0), 250);
			blocking::waitTween(t);

			_menu->setInputEnabled(true);				

			spOptionsMenu opt = OptionsMenu::instance;
			getRoot()->addChild(opt->_actor);

			opt->loop();
			opt->_actor->detach();
				
			_menu->setInputEnabled(false);

			_menu->setAlpha(0);
			t = _menu->addTween(Actor::TweenAlpha(255), 250);
			blocking::waitTween(t);

			_menu->setInputEnabled(true);
		}
		
	}
}

void MainMenu::postHiding()
{
}

void MainMenu::postShowing()
{	
}

void MainMenu::doUpdate(const UpdateState &us)
{

}

void MainMenu::clickedButton(Event *es)
{
	generateActionByEvent(es);
}