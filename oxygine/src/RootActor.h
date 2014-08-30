#pragma once
#include "oxygine_include.h"
#include "Stage.h"

namespace oxygine
{
	typedef Stage RootActor;
	typedef intrusive_ptr<Stage> spRootActor;

	/**Used for compatibility. Was renamed to getStage. Deprecated*/
	inline const spRootActor& getRoot() {return getStage();}
}