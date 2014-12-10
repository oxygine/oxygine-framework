#include "Unit.h"
#include "Game.h"

Unit::Unit():_game(0), _dead(false)
{

}

void Unit::init(const Vector2 &pos, Game *game)
{
	//initialize base
	_game = game;
	_view = new Actor;
	_view->attachTo(game);
	_view->setPosition(pos);

	//adds to global units list
	_game->_units.push_back(this);

	//should be overloaded in inherited classes
	_init();
}

const Vector2& Unit::getPosition() const
{
	return _view->getPosition();
}

void Unit::update(const UpdateState &us)
{
	//should be overloaded in inherited classes
	_update(us);
}