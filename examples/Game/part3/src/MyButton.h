#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

DECLARE_SMART(MyButton, spMyButton);
class MyButton: public Sprite
{
public:
    MyButton();

private:
    void onEvent(Event*);
};