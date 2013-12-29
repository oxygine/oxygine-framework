#include "GameScene.h"
#include "MainMenuScene.h"
#include "MyButton.h"
#include "res.h"

spGameScene GameScene::instance;

GameScene::GameScene()
{
	//create game actor
	_game = new Game;
	_game->init();
	_game->attachTo(_view);

	//create menu button 
	spActor btn = initActor(new MyButton,
		arg_resAnim = res::ui.getResAnim("menu"),
		arg_anchor = Vector2(0.5f, 0.5f),
		arg_attachTo = _view);
	
	//align it to top right
	btn->setX(_view->getWidth() - btn->getWidth()/2);
	btn->setY(btn->getHeight()/2);

	btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &GameScene::onEvent));
}

void GameScene::onEvent(Event *ev)
{
	//clicked to menu button
	changeScene(MainMenuScene::instance);
}