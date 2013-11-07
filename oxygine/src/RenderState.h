#pragma once
#include "oxygine_include.h"
#include "core/Renderer.h"

namespace oxygine
{
	class Renderer;
	class Color;
	class Event;

	typedef unsigned int dumpOptions;

	class RenderState
	{
	public:
		RenderState():renderer(0), alpha(255), clip(0)
		{
			transform.identity();
		}

		Renderer::transform transform;
		Renderer *renderer;
		unsigned char alpha;
		const RectF* clip;
	};
}