#pragma once
#include "Actor.h"
#include "Button.h"
#include "RenderState.h"

using namespace oxygine;



spButton createButtonHelper(string txt, EventCallback cb);

class Content: public Actor
{
public:
	Content():driver(0){}
	IVideoDriver *driver;

	void render(const RenderState &parentRS)
	{
		parentRS.renderer->drawBatch();

		RenderState rs = parentRS;
		Renderer renderer = *parentRS.renderer;
		renderer.cleanup();
		renderer.setDriver(driver ? driver : IVideoDriver::instance);
		rs.renderer = &renderer;
		Actor::render(rs);
	}
};

class Test: public Actor
{
public:
	Test();
	~Test();
	
	void addButton(string id, string txt);
	void updateText(string id, string txt);
	virtual void clicked(string id);
	void _clicked(Event *event);
	void back(Event *event);

	void showPopup(string text, int time = 400);

protected:
	
	float _x;
	float _y;
	spActor ui;
	Content *content;
};

extern spActor _tests;
extern Resources resources;
extern Resources resourcesUI;