#pragma once
#include "oxygine_include.h"
#include "Resource.h"
namespace oxygine
{
	class Font;

	class ResFont: public Resource
	{
	public:
		virtual Font *getFont(const char *name = 0, int size = 0) const = 0;
	};
}