#include "MaskedSprite.h"
#include "RenderState.h"
namespace oxygine
{
	spSprite MaskedSprite::getMask() const
	{
		return _mask;
	}

	void MaskedSprite::setMask(spSprite mask)
	{
		_mask = mask;
	}

	void MaskedSprite::render(const RenderState &parentRS)
	{
		if(_mask)
		{
			Renderer::transform t = getGlobalTransform(_mask);
			RectF maskDest = _mask->getDestRect();
			RectF maskSrc = _mask->getSrcRect();
			//maskDest

			const Diffuse &df = _mask->getAnimFrame().getDiffuse();
			if (df.alpha)
				parentRS.renderer->setMask(df.alpha, maskSrc, maskDest, t, true);
			else
				parentRS.renderer->setMask(df.base, maskSrc, maskDest, t, false);

		}

		Sprite::render(parentRS);

		parentRS.renderer->removeMask();
	}
}