#pragma once
#include "Actor.h"
#include "TextActor.h"
#include "MainMenu.h"
#include "ModalActor.h"
using namespace oxygine;

DECLARE_SMART(GameMenu, spGameMenu);

class ExitGame
{
public:
	ExitGame():button(false){}
	bool button;
	virtual ~ExitGame(){}
};

class GameMenu:public ModalActor
{
public:
	static spGameMenu instance;

	GameMenu();
	~GameMenu();

	void doUpdate(const UpdateState &us);

	spButtonWithText _paused;
	spButtonWithText _options;
	spButtonWithText _gotoMainMenu;
	spButtonWithText _tip;

	void clickedMainMenu(Event *es);

	void doLoop();
};