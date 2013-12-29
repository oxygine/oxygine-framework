#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

DECLARE_SMART(MyButton, spMyButton);
class MyButton: public Sprite
{
public:
	MyButton();

	void setText(const string &txt);

private:
	void onEvent(Event *);

	spTextActor _text;
};