#pragma once
#include "oxygine_include.h"
#include "TextField.h"
namespace oxygine
{
	/*
	WARNING! Class TextActor was renamed to TextField.
	TextActor name is deprecated.
	*/

	DECLARE_SMART(TextActor, spTextActor);
	class TextActor : public TextField
	{
	public:
		DECLARE_COPYCLONE_NEW(TextActor);
		OXYGINE_DEPRECATED
		TextActor(){}
	};

	inline void TextActor::copyFrom(const TextActor &src, cloneOptions opt)
	{
		TextField::copyFrom(src, opt);
	}
}