#pragma once
#include "oxygine-framework.h"
using namespace oxygine;

DECLARE_SMART(Joystick, spController);
class Joystick: public Sprite
{
public:
	Joystick();
	
	bool getDirection(Vector2 &dir) const;

private:
	void onEvent(Event *ev);
	bool _pressed;
	Vector2 _dir;

	spSprite _finger;
};