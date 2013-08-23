#pragma once
#include "Sprite.h"

namespace oxygine
{
	DECLARE_SMART(ColorRectSprite, spColorRectSprite);

	class ColorRectSprite: public Sprite
	{
	public:
		DECLARE_COPYCLONE(ColorRectSprite);
		ColorRectSprite();
		~ColorRectSprite();

		void doRender(const RenderState &rs);

	private:
	};
}