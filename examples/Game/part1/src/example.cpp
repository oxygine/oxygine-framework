#include "oxygine-framework.h"
#include "res.h"
#include "Game.h"
using namespace oxygine;

void example_preinit()
{
}

void example_init()
{
	res::load();
	
	spGame game = new Game;
	game->init();
	game->attachTo(getRoot());
}

void example_update()
{
	
}

void example_destroy()
{
	res::free();
}