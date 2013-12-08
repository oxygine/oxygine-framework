#include "Unit.h"
#include "Game.h"

Unit::Unit():_game(0)
{

}

void Unit::init(Game *game)
{
	_game = game;
	_view = new Actor;
	_view->attachTo(game);

	_init();
}

void Unit::update(const UpdateState &us)
{
	_update(us);
}