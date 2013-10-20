#include "oxygine-framework.h"

#include "GameActor.h"
#include "shared.h"

#include "MainMenu.h"
#include "GameMenu.h"
#include "Options.h"
#include "GameResult.h"

#include "Options.h"
#include "OptionsMenu.h"

#include <stdio.h>


class Exit
{
public:

};


Renderer mainRenderer;
Rect viewport;

class ExampleRootActor: public RootActor
{
public:
	ExampleRootActor()
	{
		//each mobile application should handle focus lost
		//and free/restore GPU resources
		addEventListener(RootActor::DEACTIVATE, CLOSURE(this, &ExampleRootActor::onDeactivate));
		addEventListener(RootActor::ACTIVATE, CLOSURE(this, &ExampleRootActor::onActivate));
	}

	void onDeactivate(Event *)
	{
		core::reset();
	}

	void onActivate(Event *)
	{
		core::restore();
	}
};

int updateLoop()
{
	RootActor::instance->update();

	Color c(30, 30, 30, 255);
	if (mainRenderer.begin(0, viewport, &c))
	{
		RootActor::instance->render(mainRenderer);
		mainRenderer.end();
		core::swapDisplayBuffers();
	}
	

	bool done = core::update();
	if (done)
		throw Exit();	

	return 0;
}

void run()
{
	//initialize oxygine
	core::init();

	//mainRenderer is static object 
	//VideoDriver wasn't available in mainRenderer ctor's
	mainRenderer.setDriver(IVideoDriver::instance);

	Point size = core::getDisplaySize();
	
	int width = size.x;
	int height = size.y;

	
	RootActor::instance = new ExampleRootActor();
	RootActor::instance->init(size, Point(480, 320));	

	bool high_quality = true;
	
	//init default options
	Options::instance.init("1");
	Options::instance.addValue("high_quality").set_value(high_quality);
	Options::instance.addValue("sounds").set_value(100);
	Options::instance.addValue("music").set_value(100);

	initResources();	

	GameActor::initialize();

	GameMenu::instance = new GameMenu();
	MainMenu::instance = new MainMenu();
	OptionsMenu::instance = new OptionsMenu();

	RootActor::instance->addChild(new DebugActor);
	
	bool done = false;

	Matrix view = makeViewMatrix(width, height);
	viewport = Rect(0, 0, width, height);

	Matrix proj;
	Matrix::orthoLH(proj, (float)width, (float)height, 0, 1);


	mainRenderer.setViewTransform(view);
	mainRenderer.setProjTransform(proj);
	

	blocking::setYieldCallback(updateLoop);

	try
	{
		spMainMenu mm = MainMenu::instance;
		RootActor::instance->addChild(mm->_actor);

		mm->loop();
	}
	catch(const Exit &)
	{
		printf("exit\n");
	}


	mainRenderer.cleanup();
	
	GameActor::clean();

	MainMenu::instance = 0;
	GameMenu::instance = 0;
	GameActor::instance = 0;
	OptionsMenu::instance = 0;
	
	freeResources();

	core::release();

	ObjectBase::dumpCreatedObjects();
}


#ifdef MARMALADE
int main(int argc, char* argv[])
{
	run();
	return 0;
}
#endif

#ifdef OXYGINE_SDL
#include "SDL_main.h"
extern "C"
{
	int main(int argc, char* argv[])
	{
		run();
		return 0;
	}
};
#endif