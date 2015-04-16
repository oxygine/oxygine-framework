#include "Unit.h"
#include "Game.h"

Unit::Unit(): _game(0)
{

}

void Unit::init(Game* game)
{
    //initialize base
    _game = game;
    _view = new Actor;
    _view->attachTo(game);

    //virtual method was overload in Player
    _init();
}

void Unit::update(const UpdateState& us)
{
    //virtual method was overload in Player
    _update(us);
}