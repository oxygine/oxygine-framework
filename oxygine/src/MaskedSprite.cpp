#include "MaskedSprite.h"
#include "RenderState.h"
#include "MaskedRenderer.h"

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
		if (_mask)
		{
			Renderer::transform t = getGlobalTransform(_mask);
			RectF maskDest = _mask->getDestRect();
			RectF maskSrc = _mask->getSrcRect();
			//maskDest

			const Diffuse &df = _mask->getAnimFrame().getDiffuse();


			MaskedRenderer mr(df.alpha ? df.alpha : df.base, maskSrc, maskDest, t, df.alpha ? true : false);
			RenderState rs = parentRS;
			rs.renderer = &mr;
			mr.begin(parentRS.renderer);
			Sprite::render(rs);
			mr.end();
		}
		else
		{
			Sprite::render(parentRS);
		}
	}
}