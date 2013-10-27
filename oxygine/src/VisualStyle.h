#pragma once
#include "oxygine_include.h"
#include "core/Renderer.h"
#include "Tweener.h"

namespace oxygine
{
	class RenderState;
	class VisualStyle
	{
	public:
		VisualStyle():_color(255,255,255,255), _blend(blend_premultiplied_alpha){}

		const Color&		getColor() const {return _color;}
		blend_mode			getBlendMode() const {return _blend;}

		void setColor(const Color &color){_color = color;}
		void setBlendMode(blend_mode mode){_blend = mode;}


		void _apply(const RenderState &rs);
		string dump() const;

	protected:
		Color _color;
		blend_mode _blend;
	};
}