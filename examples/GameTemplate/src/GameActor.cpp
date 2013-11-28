#include <sstream>
#include "GameActor.h"
#include "GameMenu.h"
#include "shared.h"
#include "oxygine-framework.h"

//#include "s3eAudio.h"

spGameActor GameActor::instance;

void GameActor::initialize()
{	
}

void GameActor::clean()
{
}

GameActor::GameActor()
{	
	instance = this;
	_actor->setInputEnabled(false);

	_actor->setSize(480, 320);


	spClock clock = new Clock();
	clock->setFixedStep(1000.0f/60);
	_actor->setClock(clock);
	
	


	_menu = new ButtonWithText();
	EventCallback cb = CLOSURE(this, &GameActor::generateActionByEvent);
	_menu->init("Menu", cb, Vector2(60, 30), "menu");
	_menu->setScale(Vector2(0.5f, 0.5f));
	_actor->addChild(_menu);

	_score = new TextActor;	
	TextStyle s = basicStyle;
	s.color = Color(255,128,255,255);
	s.hAlign = TextStyle::HALIGN_LEFT;
	s.vAlign = TextStyle::VALIGN_BASELINE;
	_score->setStyle(s);
	_score->setPosition(Vector2(0.0f, (float)virtualSize.y));	
	_actor->addChild(_score);

	_result = new GameResult;
	_result->init();

	_actor->setCallbackDoUpdate(CLOSURE(this, &GameActor::doUpdate));

}

GameActor::~GameActor()
{

}
void GameActor::preShowing()
{	
	_points = 0;	
	_score->setAlpha(0);
	_score->addTween(Actor::TweenAlpha(255), 300);	
}

void GameActor::postShowing()
{	
	spSprite go = new Sprite();
	go->setResAnim(gameResources.getResAnim("go"));
	go->setAnchor(Vector2(0.5f, 0.5f));
	go->setScale(Vector2(2.0f, 2.0f));	
	go->setPosition(virtualSize/2);
	spTweenQueue queue = new TweenQueue();
	queue->add(Actor::TweenScale(Vector2(1.0f, 1.0f)), 200);
	queue->add(Actor::TweenAlpha(0), 400, 3, true);
	queue->add(Actor::TweenAlpha(255), 400);
	go->addTween(queue);
	//queue->setDetachActor(true);

	_actor->addChild(go);
}

void GameActor::doLoop()
{
	while (1)
	{
		string action = waitAction();

		if (action == "menu")
		{
			_result->setup((int)_points);

			_actor->addChild(_result->_actor);
			_result->loop();
			_result->_actor->detach();
			break;
		}
	}	
}


void GameActor::doUpdate(const UpdateState &us)
{
	int points = int(_points);
	_points += (us.dt/1000.0f) * 5;

	if (points != int(_points))
	{
		char str[255];
		safe_sprintf(str, "Score: %d", (int)_points);
		_score->setText(str);
	}	
}