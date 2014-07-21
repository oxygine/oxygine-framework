#pragma once
#include "oxygine_include.h"
#include "TextField.h"
namespace oxygine
{
	/*
	WARNING! Class TextActor was renamed to TextField.
	TextActor name is deprecated.
	*/

	class TextActor : public TextField
	{
	public:
		OXYGINE_DEPRECATED
		TextActor(){}
	};
}