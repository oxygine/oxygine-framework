#pragma once
#include "oxygine_include.h"

namespace oxygine
{
	struct serializedata;
	struct deserializedata;

	class Serializable
	{
	public:
		virtual void serialize(serializedata*){}
		virtual void deserialize(const deserializedata*){}
	};
}