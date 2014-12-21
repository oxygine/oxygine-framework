#pragma once
#include "Actor.h"
#include "Button.h"
#include "RenderState.h"
#include "STDRenderer.h"
using namespace oxygine;
using namespace std;


spButton createButtonHelper(spButton, std::string txt, EventCallback cb);

class Content: public Actor
{
public:
	Content():driver(0){}
	IVideoDriver *driver;

	void render(const RenderState &parentRS)
	{
		
		parentRS.renderer->drawBatch();

		RenderState rs = parentRS;
		STDRenderer renderer(driver ? driver : IVideoDriver::instance);
		renderer.begin(parentRS.renderer);
		rs.renderer = &renderer;
		Actor::render(rs);
		renderer.end();
	}	
};

class Test: public Actor
{
public:
	Test();
	~Test();

	struct toggle
	{
		string text;
		int value;
		const void *data;
		toggle(){}
		toggle(const char *text_, int v_ = 0, const void *data_ = 0):text(text_), value(v_), data(data_){}

	};
	
	spButton addButton(string id, string txt);
	void addToggle(string id, const toggle *t, int num);
	void updateText(string id, string txt);
	virtual void clicked(string id){}
	virtual void toggleClicked(string id, const toggle *data){}
	void _clicked(Event *event);
	void _toggleClicked(Event *event);
	void back(Event *event);

	void notify(string text, int time = 400);

protected:
	void notifyDone(Event *ev);
	
	float _x;
	float _y;
	spActor ui;
	Content *content;
	enum {MAX_NOTIFIES = 8};
	int _notifies[MAX_NOTIFIES];
};

extern spActor _tests;
extern Resources resources;
extern Resources resourcesUI;