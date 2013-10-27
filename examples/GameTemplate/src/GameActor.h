#pragma once
#include "oxygine-framework.h"
#include "ModalActor.h"
#include "GameResult.h"
using namespace oxygine;


DECLARE_SMART(GameActor, spGameActor);

class GameActor: public ModalActor
{
public:
	static spGameActor instance;

	GameActor();
	~GameActor();

	

	static void initialize();
	static void clean();

	void doLoop();
	void postShowing();
	void preShowing();


private:
	void doUpdate(const UpdateState &us);

	spButtonWithText _menu;
	spTextActor _score;

	float _points;


	spGameResult _result;
};