#pragma once
#include "ModalActor.h"
#include "MainMenu.h"
#include "ColorRectSprite.h"

DECLARE_SMART(GameResult, spGameResult);
class GameResult: public ModalActor
{
public:
	static spGameResult instance;

	GameResult();
	~GameResult();

	void init();
	void setup(int score);
	void doLoop();

private:
	spColorRectSprite _bg;

	spButtonWithText _score;
	spButtonWithText _done;
};