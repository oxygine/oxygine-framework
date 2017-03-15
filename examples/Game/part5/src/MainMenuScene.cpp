#include "MainMenuScene.h"
#include "res.h"
#include "MyButton.h"
#include "GameScene.h"

spMainMenuScene MainMenuScene::instance;

MainMenuScene::MainMenuScene()
{
    //create background
    spSprite sky = new Sprite;
    sky->setResAnim(res::ui.getResAnim("bg"));
    sky->attachTo(_view);

    /*
    'initActor' is syntax sugar for

    spSprite logo = new Sprite;
    logo->setResAnim(res::ui.getResAnim("logo2")),
    logo->attachTo(_view);
    */

    //create logo
    spSprite logo = initActor(new Sprite,
                              arg_resAnim = res::ui.getResAnim("logo2"),
                              arg_attachTo = _view);
    logo->setPosition(_view->getSize() - logo->getSize());


    /*
    spSprite btn = new MyButton;
    btn->setResAnim(res::ui.getResAnim("play")),
    btn->setAnchor(Vector2(0.5f, 0.5f));
    btn->setPosition(_view->getSize()/2);
    btn->attachTo(_view);
    */

    //create play button at center
    spSprite btn = initActor(new MyButton,
                             arg_name = "play",
                             arg_resAnim = res::ui.getResAnim("play"),
                             arg_anchor = Vector2(0.5f, 0.5f),
                             arg_pos = _view->getSize() / 2,
                             arg_attachTo = _view);

    //handle click to button
    btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainMenuScene::onEvent));


    //create menu button
    btn = initActor(new MyButton,
                    arg_name = "exit",
                    arg_resAnim = res::ui.getResAnim("menu"),
                    arg_anchor = Vector2(0.5f, 0.5f),
                    arg_attachTo = _view);

    //align it to top right
    btn->setX(_view->getWidth() - btn->getWidth() / 2);
    btn->setY(btn->getHeight() / 2);

    btn->addEventListener(TouchEvent::CLICK, CLOSURE(this, &MainMenuScene::onEvent));
}

void MainMenuScene::onEvent(Event* ev)
{
    string id = ev->currentTarget->getName();
    if (id == "exit")
    {
        //request quit
        core::requestQuit();
    }

    if (id == "play")
    {
        //clicked to play button
        //change scene
        changeScene(GameScene::instance);
    }
}