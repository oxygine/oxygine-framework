#pragma once
#include "oxygine-framework.h"
#include "Unit.h"

DECLARE_SMART(Rocket, spRocket);
class Rocket: public Unit
{
public:
	Rocket(const Vector2 &dir);
	
protected:
	void _init();
	void _update(const UpdateState &us);
	void explode();

	Vector2 _dir;
};