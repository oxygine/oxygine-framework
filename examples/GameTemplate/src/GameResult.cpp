#include "GameResult.h"
#include <sstream>
#include "oxygine-framework.h"
#include "shared.h"

spGameResult GameResult::instance;
GameResult::GameResult()
{	
}

GameResult::~GameResult()
{

}

void GameResult::init()
{
	ColorRectSprite *p = new ColorRectSprite;
	_bg = p;
	_bg->setSize(getRoot()->getSize());
	_bg->setColor(Color(30, 30, 30, (int)(255 * 0.9f)));
	_actor->addChild(_bg);

	_score = new ButtonWithText;
	_done = new ButtonWithText;

	EventCallback cb;
	Vector2 pos(virtualSize.x/2.0f, 50);
	_score->init("", cb, pos, "score");
	_actor->addChild(_score);
	

	pos.y = 250;
	cb = CLOSURE(this, &Modal::generateActionByEvent);
	_done->init(getString("result", "done"), cb, pos, "done");
	_actor->addChild(_done);
}

void GameResult::setup(int score)
{
	char str[255];
	safe_sprintf(str, "%d", score);
	_score->setText(str);
}


void GameResult::doLoop()
{
	while(1)
	{
		string action = waitAction();

		if (!action.empty())
		{
			break;
		}
	}
}