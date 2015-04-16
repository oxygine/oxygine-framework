#include "oxygine-framework.h"
#include "res.h"
#include "Game.h"
#include "MainMenuScene.h"
#include "GameScene.h"
#include "GameMenu.h"
using namespace oxygine;


void example_preinit()
{
}

void example_init()
{
    //load resources
    res::load();

    //create all scenes
    GameMenu::instance = new GameMenu;
    GameScene::instance = new GameScene;
    MainMenuScene::instance = new MainMenuScene;

    //show main menu
    MainMenuScene::instance->show();
}

void example_update()
{

}

void example_destroy()
{
    MainMenuScene::instance = 0;
    GameScene::instance = 0;
    GameMenu::instance = 0;

    res::free();
}