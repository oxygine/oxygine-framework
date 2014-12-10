#pragma once
#include "Scene.h"

DECLARE_SMART(GameMenu, spGameMenu);
class GameMenu: public Scene
{
public:
	static spGameMenu instance;

	GameMenu();
	const string& getLastClicked() const {return _lastClicked;}

private:
	void onEvent(Event *ev);
	void showButtons(Event *ev);
	void _show();

	spBox9Sprite _bg;
	spActor _buttons;
	string _lastClicked;
};