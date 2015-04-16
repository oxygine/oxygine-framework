#include "GameMenu.h"
#include "MainMenuScene.h"
#include "MyButton.h"
#include "res.h"

spGameMenu GameMenu::instance;

GameMenu::GameMenu()
{
    //initialize dialog background
    _bg = initActor(new Box9Sprite,
                    arg_attachTo = _view,
                    arg_resAnim = res::ui.getResAnim("box9"),
                    arg_pos = _view->getSize() / 2 - Vector2(300, 300) / 2);

    _bg->setGuides(30, 30, 30, 30);


    //create holder for button and text
    _buttons = new Actor;
    _buttons->attachTo(_view);

    float y = _view->getHeight() / 2 - 80;

    //initialize TextStyle for TextField
    //TextStyle is plain struct with "setting" for Text
    TextStyle style;
    style.font = res::ui.getResFont("big")->getFont();
    //vertical align
    style.vAlign = TextStyle::VALIGN_BOTTOM;
    //horizontal align
    style.hAlign = TextStyle::HALIGN_CENTER;

    spTextField paused = initActor(new TextField,
                                   arg_style = style,
                                   //colored text by "html" tags
                                   arg_htmlText = "Paused<div c='00FF00'>!</div>",
                                   arg_x = _view->getWidth() / 2,
                                   arg_y = y,
                                   arg_attachTo = _buttons,
                                   arg_alpha = 128);

    //animate with infinity loops "Paused!" text
    paused->addTween(Actor::TweenAlpha(255), 1200, -1, true);
    paused->addTween(Actor::TweenScale(1.1f), 1200, -1, true);

    y += 70;


    //'buttons' image has 2 columns and 5 rows
    //game actually uses only 2 selected randomly
    const ResAnim* btns = res::ui.getResAnim("buttons");
    //create 2 buttons
    const char* txt[2] = {"Continue", "Exit"};
    for (int i = 0; i < 2; ++i)
    {
        //select random frame of image
        const AnimationFrame& frame = btns->getFrame(rand() % btns->getColumns(), rand() % btns->getRows());

        //MyButton is user class inherited from Sprite
        spMyButton button = initActor(new MyButton,
                                      arg_resAnim = frame,
                                      arg_anchor = Vector2(0.5f, 0.5f),
                                      arg_attachTo = _buttons);

        button->setX(_view->getWidth() / 2);
        button->setY(y);
        button->setText(txt[i]);

        y += button->getHeight() + 10;

        button->setName(txt[i]);
        //handle click on button
        //each Object could have unique name. In this example button has the same name as text
        button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &GameMenu::onEvent));
    }
}

void GameMenu::_show()
{
    //before showing dialog hide buttons
    _buttons->setAlpha(0);

    //animate background
    //set default small size
    _bg->setSize(_bg->getAnimFrame().getSize());

    //and modify height and then width by tween
    spTweenQueue tq = new TweenQueue;
    tq->add(Actor::TweenHeight(300), 500);
    tq->add(Actor::TweenWidth(300), 500);
    //and show buttons when done
    tq->addDoneCallback(CLOSURE(this, &GameMenu::showButtons));
    _bg->addTween(tq);
}

void GameMenu::showButtons(Event* ev)
{
    //tween activated from GameMenu::_show is done
    //fade in buttons and text
    _buttons->addTween(Actor::TweenAlpha(255), 300);
}

void GameMenu::onEvent(Event* ev)
{
    //button clicked
    //remember it's name. It would asked later from GameScene.cpp
    _lastClicked = ev->currentTarget->getName();

    //hide dialog
    hide();
}