#include "Game.h"
#include "Joystick.h"
#include "Player.h"
#include "res.h"

Game::Game()
{
	
}

void Game::init()
{
	//scene layer would have size of display
	setSize(getRoot()->getSize());

	//create background
	spSprite sky = new Sprite;
	sky->setResAnim(res::ui.getResAnim("sky"));
	sky->attachTo(this);
	
	//create player ship
	_player = new Player;
	_player->init(this);

	//create virtual joystick
	_move = new Joystick;
	_move->attachTo(this);
	_move->setY(getHeight() - _move->getHeight());
}

void Game::doUpdate(const UpdateState &us)
{
	//doUpdate is virtual method of Actor
	//it is being called each frame

	//update player each frame
	_player->update(us);
}