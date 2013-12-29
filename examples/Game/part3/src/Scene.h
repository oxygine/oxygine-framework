#pragma once
#include "oxygine-framework.h"
#include <vector>
using namespace oxygine;

DECLARE_SMART(Scene, spScene);
class Scene: public Object
{
public:
	Scene();

	void changeScene(spScene next);

	spActor getView() const {return _view;}

	spActor _view;
};



