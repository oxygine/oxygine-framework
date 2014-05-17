/**
Attention!
This file has Oxygine initialization stuff.
If you just started you don't need to understand it exactly you could check it later.
You could start from example.cpp and example.h it has main functions being called from there
*/
#include <stdio.h>
#include "core/Renderer.h"
#include "RootActor.h"
#include "DebugActor.h"

#include "example.h"


using namespace oxygine;

Renderer renderer;
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

//called each frame
int mainloop()
{
	example_update();
	//update our rootActor
	//Actor::update would be called also for children
	getRoot()->update();

	Color clear(33, 33, 33, 255);
	//start rendering and clear viewport
	if (renderer.begin(0, viewport, &clear))
	{
		//begin rendering from RootActor.
		getRoot()->render(renderer);
		//rendering done
		renderer.end();

		core::swapDisplayBuffers();
	}


	//update internal components
	//all input events would be passed to RootActor::instance.handleEvent
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

#if OXYGINE_SDL
	//we could setup initial window size on SDL builds
	//desc.w = 960;
	//desc.h = 660;
	//marmalade settings could be changed from emulator's menu
#endif

	example_preinit();
	core::init(&desc);
	

	//create RootActor. RootActor is a root node
	RootActor::instance = new ExampleRootActor();
	Point size = core::getDisplaySize();
	getRoot()->init(size, size);

	//DebugActor is a helper node it shows FPS and memory usage and other useful stuff
	DebugActor::initialize();

	//create and add new DebugActor to root actor as child
	getRoot()->addChild(new DebugActor());



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

	/**releases all internal components and RootActor*/
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
                     LPSTR lpCmdLine,int nCmdShow)
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

#ifdef __FLASHPLAYER__
int main(int argc, char* argv[])
{
	printf("test\n");
	run();
	return 0;
}
#endif
