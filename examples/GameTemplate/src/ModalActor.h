#pragma once
#include "Modal.h"

using namespace oxygine;
DECLARENS_SMART(oxygine, Actor, spActor);

class ModalActor: public Modal
{
public:
	ModalActor();
	~ModalActor();

	void showing();
	void hiding();

	void doShowing();
	void doHiding();



	timeMS _timefadeIn;
	timeMS _timeFadeOut;
	spActor _actor;
};
