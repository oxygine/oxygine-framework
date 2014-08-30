#include "oxygine-framework.h"
#include "res.h"
#include "Game.h"
#include "MainMenuScene.h"
#include "GameScene.h"
using namespace oxygine;


void example_preinit()
{
}

void example_init()
{
	//load resources
	res::load();

	GameScene::instance = new GameScene;
	MainMenuScene::instance = new MainMenuScene;
	getStage()->addChild(MainMenuScene::instance->getView());
}

void example_update()
{

}

void example_destroy()
{
	MainMenuScene::instance = 0;
	GameScene::instance = 0;

	res::free();
}