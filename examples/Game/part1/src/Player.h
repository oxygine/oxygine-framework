#pragma once
#include "Unit.h"

DECLARE_SMART(Player, spPlayer);
class Player: public Unit
{
public:

	
protected:
	void _init();
	void _update(const UpdateState &us);

	spSprite _engine;	
	spSprite _ship;	
};
