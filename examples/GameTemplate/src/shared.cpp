#include "oxygine-framework.h"
#include "shared.h"

#ifdef MARMALADE
#include "s3eDevice.h"
#include "IwUTF8.h"
#endif

#include <stdio.h>

Point virtualSize(480, 320);
Resources gameResources;
TextStyle basicStyle; 

pugi::xml_node strings;
pugi::xml_document doc;

void initResources()
{	
	float scale = RootActor::instance->getScaleX();
	/*
	bool hd = scale > 1.5f;

	if (hd)
	{
		file::setExtendedFolder("hd");
	}
	*/

	gameResources.loadXML("xmls/res.xml");
	
	Font *fontMain = gameResources.getResFont("main24")->getFont();
	
	basicStyle.color = Color(64, 0, 128, 255);
	basicStyle.font = fontMain;

	
	//if (hd)	
	//	basicStyle.font->setScaleFactor(2.0f);

	const char *locale_ = "unknown";

#ifdef MARMALADE
	locale_ = s3eDeviceGetString(S3E_DEVICE_LOCALE);
#endif

	printf("locale: %s\n", locale_);
	file::buffer fb;
	string fname = string("strings_") + locale_ +".xml";
	file::read(fname.c_str(), fb);
	if (!fb.getSize())
	{
		file::read("strings_en_GB.xml", fb);
	}

	doc.load_buffer(&fb.data[0], fb.data.size());
	strings = doc.first_child();
}

void freeResources()
{
	gameResources.free();
}

std::string getString(const char *group, const char *id)
{
	bool b = strings.empty();
	pugi::xml_node gr = strings.child(group);
	b = gr.empty();
	pugi::xml_node v = gr.child(id); 
	b = v.empty();
	return v.first_child().value();
}
