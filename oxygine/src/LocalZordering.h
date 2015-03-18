#pragma once

#include "Actor.h"

namespace oxygine
{
	DECLARE_SMART( LocalZordering, spLocalZordering )
	class LocalZordering : public Actor
	{
	public:
		virtual bool asLocalZordering();
	};
}