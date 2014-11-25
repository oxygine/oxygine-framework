#pragma once
#include "oxygine_include.h"
#include "core/Renderer.h"

namespace oxygine
{
	class Renderer;
	class Color;
	class Event;
	
	class RState
	{
	public:
		RState():alpha(255), clip(0)
		{
			transform.identity();
		}

		Renderer::transform transform;
		unsigned char alpha;
		const RectF* clip;
	};

	class RenderState: public RState
	{
	public:
		RenderState():renderer(0){}
		Renderer *renderer;		
	};
}