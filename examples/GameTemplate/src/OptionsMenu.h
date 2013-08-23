#pragma once
#include "ModalActor.h"
#include "MainMenu.h"
using namespace oxygine;


DECLARE_SMART(OptionsMenu, spOptionsMenu);


class OptionsMenu: public ModalActor
{
public:

	static spOptionsMenu instance;

	OptionsMenu();
	~OptionsMenu();

	bool isHighQuality(){return _quality->enabled;}

	void doShowing();
	void doHiding();

private:
	void clickedButton(Event *event);
	void doLoop();

	void switchButton(spButtonWithText btn);


	spButtonWithText _music;
	spButtonWithText _sounds;
	spButtonWithText _quality;
	spButtonWithText _back;		
};