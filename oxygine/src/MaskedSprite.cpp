#include "MaskedSprite.h"
#include "RenderState.h"
#include "MaskedRenderer.h"
#include "Serialize.h"

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
        if (_mask && _mask->getAnimFrame().getDiffuse().base)
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
			_Sprite::render(rs);
			mr.end();
		}
		else
		{
			_Sprite::render(parentRS);
		}
	}

    void MaskedSprite::serialize(serializedata* data)
    {
        _Sprite::serialize(data);
        data->node.set_name("MaskedSprite");
    }

    void MaskedSprite::deserialize(const deserializedata* data)
    {
        _Sprite::deserialize(data);
    }

	void MaskedSprite::deserializeLink(const deserializeLinkData* data)
	{
        const char *id = data->node.attribute("mask").as_string(0);
        if (!id)
            return;

        spSprite mask = data->root->getDescendantT<Sprite>(id, ep_ignore_error);
		setMask(mask);
	}
}
