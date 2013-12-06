#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "core/Renderer.h"
#include "Tweener.h"

namespace oxygine
{
	class RenderState;
	class VisualStyle
	{
	public:	

		VisualStyle();

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

	class VStyleActor: public Actor
	{
	public:
		DECLARE_COPYCLONE(VStyleActor);
		VStyleActor(){}

		blend_mode				getBlendMode() const {return _vstyle.getBlendMode();}
		const Color&			getColor() const {return _vstyle.getColor();}

		void					setColor(const Color &color){_vstyle.setColor(color);}
		void					setBlendMode(blend_mode mode){_vstyle.setBlendMode(mode);}


		typedef GetSet<Color, const Color &, VStyleActor, &VStyleActor::getColor, &VStyleActor::setColor>	TweenColor;

	protected:
		VisualStyle _vstyle;
	};
}