#pragma once
#include "Unit.h"

DECLARE_SMART(Enemy, spEnemy);
class Enemy: public Unit
{
public:

	void explode();

private:
	void _init();
	void _update(const UpdateState &us);
	int _hp;
};