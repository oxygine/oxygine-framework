#pragma once
#include "Scene.h"
#include "Game.h"

DECLARE_SMART(GameScene, spGameScene);
class GameScene: public Scene
{
public:
    static spGameScene instance;
    GameScene();


private:
    void onEvent(Event* ev);
    spGame _game;
};
