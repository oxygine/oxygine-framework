#include "oxygine-framework.h"
#include "Options.h"
#include "shared.h"
#include "GameActor.h"
#include "GameMenu.h"
#include "MainMenu.h"
#include "OptionsMenu.h"
using namespace oxygine;

int mainloop();//declared in entry_point.cpp

class Exit
{
public:

};

void example_preinit()
{
}

void example_init()
{
	Point size = core::getDisplaySize();

	//getRoot()->init(size, Point(480, 320));	

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

	blocking::setYieldCallback(mainloop);

	try
	{
		spMainMenu mm = MainMenu::instance;
		getRoot()->addChild(mm->_actor);

		mm->loop();
	}
	catch(const Exit &)
	{
		printf("exit\n");
	}

}



void example_update()
{


}

void example_destroy()
{
	GameActor::clean();

	MainMenu::instance = 0;
	GameMenu::instance = 0;
	GameActor::instance = 0;
	OptionsMenu::instance = 0;

	freeResources();
}