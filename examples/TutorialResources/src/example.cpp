#include "oxygine-framework.h"
using namespace oxygine;

Resources animals;

void example_preinit()
{

}
void example_init()
{
    animals.loadXML("animals.xml");

    spSprite dog = new Sprite;
    dog->setResAnim(animals.getResAnim("dog"));
    dog->setPosition(200, 300);
    dog->attachTo(getStage());

    spSprite cat = new Sprite;
    cat->setResAnim(animals.getResAnim("cat"));
    cat->setPosition(600, 300);
    cat->attachTo(getStage());
}

void example_update()
{

}

void example_destroy()
{
    animals.free();
}