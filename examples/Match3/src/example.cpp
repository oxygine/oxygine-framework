#include "oxygine-framework.h"

#include "Board.h"
#include "shared.h"

using namespace oxygine;


Board board;

void example_preinit()
{

}

void example_init()
{
    //load xml file with resources definition
    res.loadXML("resources.xml");

    spSprite bg = new Sprite;
    bg->setResAnim(res.getResAnim("bg"));
    bg->attachTo(getStage());
    bg->setSize(getStage()->getSize());

    board.init(6, 5);

    spActor view = board.getView();
    view->setScale(getStage()->getHeight()/view->getHeight());
    view->setPosition(getStage()->getSize() / 2 - view->getScaledSize() / 2);
    getStage()->addChild(view);
}

void example_destroy()
{
    board.free();
    res.free();
}
void example_update()
{

}