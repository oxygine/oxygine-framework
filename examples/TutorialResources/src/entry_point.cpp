#include <stdio.h>

#include "core/Renderer.h"
#include "RootActor.h"
#include "DebugActor.h"

#include "example.h"


using namespace oxygine;

Renderer renderer;
Rect viewport;

int mainloop()
{
	//update our rootActor
	//Actor::update would be called also for children
	RootActor::instance->update();

	Color clear(33, 33, 33, 255);
	//start rendering and clear viewport
	renderer.begin(0, viewport, &clear);
	//begin rendering from RootActor. 
	RootActor::instance->render(renderer);
	//rendering done
	renderer.end();

	//update internal components
	//all input events would be passed to RootActor::instance.handleEvent
	//if done is true then User requests quit from app.
	bool done = core::update();

	return done ? 1 : 0;
}

//it is application entry point
void run()
{	
	//initialize oxygine's internal stuff
	core::init();
	
	
	//create RootActor. RootActor is a root node
	RootActor::instance = new RootActor();

	
	Point size = core::getDisplaySize();

	//initialize it
	//first argument is real display size of device.
	//second is your "virtual" preferred size. You could change it to any size you need
	//VirtualWidth and VirtualHeight are defined in example.h	
	RootActor::instance->init(size, 
		Point(VirtualWidth, VirtualHeight));
	

	//DebugActor is a helper node it shows FPS and memory usage
	DebugActor::initialize();


	//create and add new DebugActor to root actor as child
	RootActor::instance->addChild(new DebugActor());


	//it is view and projection matrix  initialization stuff
	
	Matrix view = makeViewMatrix(size.x, size.y); //Returns View matrix where Left Top corner is (0,0), and right bottom is (w,h)

	viewport = Rect(0, 0, size.x, size.y);

	Matrix proj;
	//initialize projection matrix
	Matrix::orthoLH(proj, (float)size.x, (float)size.y, 0, 1);
	
	//initialize Renderer
	//Renderer is class helper for rendering primitives and batching them
	//Renderer is lightweight class you could create it many of times
	//for example if you need to render something into RenderTarget (FBO)
	renderer.setDriver(IVideoDriver::instance);
	renderer.setViewTransform(view);
	renderer.setProjTransform(proj);


	//initialize this example stuff. check example.cpp
	example_init();


	bool done = false;	

	//here is main game loop
    while (1)
    {
		int done = mainloop();
		if (done)
			break;
    }
	//so user closed application
	
	//lets dump all created objects into Output
	//all created and not freed resources would be displayed
	ObjectBase::dumpCreatedObjects();


	//lets cleanup everything right now and call ObjectBase::dumpObjects() again
	//we need to free all allocated resources and delete all created actors
	//yes, actor is smart pointer and actually you don't need it remove by hands
	//but now we want delete it forcedly

	//check example.cpp
	example_destroy();	
	

	renderer.cleanup();

	/**releases all internal components and RootActor*/
	core::release();

	//dump list should be empty now
	//we deleted everything and could be sure that there aren't any memory leaks
	ObjectBase::dumpCreatedObjects();

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
extern "C"
{
	int SDL_main(int argc, char* argv[])
	{
		run();
		return 0;
	}
};
#endif

#ifdef __FLASHPLAYER__
int main(int argc, char* argv[])
{
	printf("test\n");
	run();
	return 0;
}
#endif