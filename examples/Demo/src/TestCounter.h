#pragma once
#include "test.h"
#include "Counter.h"

class TestCounter: public Test
{
public:
    TestCounter()
    {
        spSprite bg = new Sprite;
        bg->setResAnim(resources.getResAnim("score_table"));
        bg->setPosition(getStage()->getSize() / 2 - bg->getSize() / 2);
        content->addChild(bg);


        //Thanks to Dmitry Volokitin for Counter class  source code and assets used in this Test

        counter = new Counter;
        counter->setSize(702, 80);
        counter->init(&resources, 88);
        counter->reset();
        counter->setValue(1000);
        counter->setPosition(bg->getPosition() + Vector2(25, 20));
        content->addChild(counter);
    }

    spCounter counter;
};