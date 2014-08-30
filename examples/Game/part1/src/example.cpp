#include "oxygine-framework.h"
#include "res.h"
#include "Game.h"
using namespace oxygine;

void example_preinit()
{
}

void example_init()
{
	//load resources
	res::load();
	
	//create scene with game
	spGame game = new Game;
	game->init();
	game->attachTo(getStage());
}

void example_update()
{
	
}

void example_destroy()
{
	res::free();
}