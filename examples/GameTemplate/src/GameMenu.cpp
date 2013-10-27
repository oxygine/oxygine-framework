#include "GameMenu.h"
#include "RootActor.h"
#include "GameActor.h"
#include "shared.h"
#include "blocking.h"

spGameMenu GameMenu::instance;

GameMenu::GameMenu()
{
	_actor->setSize(getRoot()->getSize());

	Vector2 size(_actor->getSize());

	EventCallback cb;

	Vector2 pos(virtualSize.x/2.0f, 50);

	_paused = new ButtonWithText;
	_paused->init(getString("game_menu", "paused"), cb, pos, "paused");
	_actor->addChild(_paused);

	_tip = new ButtonWithText;
	_tip->init(getString("game_menu", "message"), cb, pos, "tip");
	_actor->addChild(_tip);

	//_options.init(L"Options", 100, cb, "options");
	//addChild(&_options);

	cb = CLOSURE(this, &GameMenu::clickedMainMenu);

	_gotoMainMenu = new ButtonWithText;
	_gotoMainMenu->init(getString("game_menu", "main_menu"), cb, pos, "main_menu");

	_actor->addChild(_gotoMainMenu);

	_timeFadeOut = 50;
}

void GameMenu::clickedMainMenu(Event *es)
{
	return generateActionByEvent(es);
}

GameMenu::~GameMenu()
{

}



void GameMenu::doLoop()
{
	while(1)
	{
		string action = waitAction();

		if (action == "play")
			break;

		
		blocking::yield();//avoid unhandled exception bug
		if (action == "main_menu")
		{
			throw ExitGame();
		}
	}	
}

void GameMenu::doUpdate(const UpdateState &us)
{

}