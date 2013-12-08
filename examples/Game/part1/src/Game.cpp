#include "Game.h"
#include "Joystick.h"
#include "Player.h"
#include "res.h"

Game::Game()
{
	
}

void Game::init()
{
	setSize(getRoot()->getSize());

	spSprite sky = new Sprite;
	sky->setResAnim(res::ui.getResAnim("sky"));
	sky->attachTo(this);
	
	_player = new Player;
	_player->init(this);

	_move = new Joystick;
	_move->attachTo(this);
	_move->setY(getHeight() - _move->getHeight());
}

void Game::doUpdate(const UpdateState &us)
{
	_player->update(us);
}