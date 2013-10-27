#pragma once
#include "oxygine_include.h"
#include "Sprite.h"
namespace oxygine
{
	DECLARE_SMART(MaskedSprite, spMaskedSprite);
	class MaskedSprite: public Sprite
	{
	public:
		spSprite getMask() const;
		void setMask(spSprite);	


	protected:
		void render(const RenderState &parentRS);

	private:
		spSprite _mask;
	};
}