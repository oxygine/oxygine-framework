#pragma once
#include "Actor.h"
#include "TextActor.h"
#include "ModalActor.h"
#include "Button.h"
using namespace oxygine;

DECLARE_SMART(ButtonWithText, spButtonWithText);
class ButtonWithText: public Button
{
public:
	ButtonWithText();
	~ButtonWithText();

	bool enabled;//used for options

	void init(const string &text, EventCallback &cb, const Vector2 &pos, const string &name);
	void setText(const string &str);

private:
	spTextActor _text;
};

DECLARE_SMART(MainMenu, spMainMenu);
DECLARE_SMART(GameActor, spGameActor);


class MainMenu:public ModalActor
{
public:
	static spMainMenu instance;

	MainMenu();
	~MainMenu();

	void postShowing();
	void postHiding();

	void doLoop();

	void clickedButton(Event *es);

	void doUpdate(const UpdateState &us);

	//string waitAction();

	spButtonWithText _play;
	spButtonWithText _options;
	spActor _menu;


private:
	void fadeOutDone(Tween *tween, Actor *actor);
	string _action;

};