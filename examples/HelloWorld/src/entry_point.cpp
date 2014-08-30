/**
Attention!
This file has Oxygine initialization stuff.
If you just started you don't need to understand it exactly you could check it later.
You could start from example.cpp and example.h it has main functions being called from there
*/
#include <stdio.h>
#include "core/Renderer.h"
#include "Stage.h"
#include "DebugActor.h"

#include "example.h"



using namespace oxygine;

Renderer renderer;
Rect viewport;


class ExampleStage : public Stage
{
public:
	ExampleStage()
	{
		//each mobile application should handle focus lost
		//and free/restore GPU resources
		addEventListener(Stage::DEACTIVATE, CLOSURE(this, &ExampleStage::onDeactivate));
		addEventListener(Stage::ACTIVATE, CLOSURE(this, &ExampleStage::onActivate));
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

//called each frame
int mainloop()
{
	example_update();
	//update our stage
	//Actor::update would be called also for children
	getStage()->update();

	Color clear(33, 33, 33, 255);
	//start rendering and clear viewport
	if (renderer.begin(0, viewport, &clear))
	{
		//begin rendering from Stage.
		getStage()->render(renderer);
		//rendering done
		renderer.end();

		core::swapDisplayBuffers();
	}


	//update internal components
	//all input events would be passed to Stage::instance.handleEvent
	//if done is true then User requests quit from app.
	bool done = core::update();

	return done ? 1 : 0;
}

//it is application entry point
void run()
{
	ObjectBase::__startTracingLeaks();

	//initialize oxygine's internal stuff
	core::init_desc desc;

#if OXYGINE_SDL || OXYGINE_EMSCRIPTEN
	//we could setup initial window size on SDL builds
	desc.w = 960;
	desc.h = 640;
	//marmalade settings could be changed from emulator's menu
#endif


	example_preinit();
	core::init(&desc);


	//create Stage. Stage is a root node
	Stage::instance = new ExampleStage();
	Point size = core::getDisplaySize();
	getStage()->init(size, size);

	//DebugActor is a helper node it shows FPS and memory usage and other useful stuff
	DebugActor::initialize();

	//create and add new DebugActor to stage as child
	getStage()->addChild(new DebugActor());



	Matrix view = makeViewMatrix(size.x, size.y);

	viewport = Rect(0, 0, size.x, size.y);

	Matrix proj;
	//initialize projection matrix
	Matrix::orthoLH(proj, (float)size.x, (float)size.y, 0, 1);

	//Renderer is class helper for rendering primitives and batching them
	//Renderer is lightweight class you could create it many of times
	renderer.setDriver(IVideoDriver::instance);

	//initialization view and projection matrix
	//where Left Top corner is (0, 0), and right bottom is (width, height)
	renderer.initCoordinateSystem(size.x, size.y);

	//initialize this example stuff. see example.cpp
	example_init();

#ifdef EMSCRIPTEN
	/*
	if you build for Emscripten mainloop would be called automatically outside. 
	see emscripten_set_main_loop below
	*/	
	return;
#endif

	bool done = false;

	//here is main game loop
	while (1)
	{
		int done = mainloop();
		if (done)
			break;
	}
	//so user want to leave application...

	//lets dump all created objects into log
	//all created and not freed resources would be displayed
	ObjectBase::dumpCreatedObjects();

	//lets cleanup everything right now and call ObjectBase::dumpObjects() again
	//we need to free all allocated resources and delete all created actors
	//all actors/sprites are smart pointer objects and actually you don't need it remove them by hands
	//but now we want delete it by hands

	//check example.cpp
	example_destroy();


	renderer.cleanup();

	/**releases all internal components and Stage*/
	core::release();

	//dump list should be empty now
	//we deleted everything and could be sure that there aren't any memory leaks
	ObjectBase::dumpCreatedObjects();

	ObjectBase::__stopTracingLeaks();
	//end
}

#ifdef __S3E__
int main(int argc, char* argv[])
{
	run();
	return 0;
}
#endif


#ifdef OXYGINE_SDL
#ifdef __MINGW32__
int WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	run();
	return 0;
}
#else
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
#endif

#ifdef EMSCRIPTEN
#include <emscripten.h>

void one(){ mainloop(); }

int main(int argc, char* argv[])
{
	run();
	emscripten_set_main_loop(one, 0, 0);
	return 0;
}
#endif
