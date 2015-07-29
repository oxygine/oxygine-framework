#include "MaskedSprite.h"
#include "RenderState.h"
#include "MaskedRenderer.h"
#include "Serialize.h"

namespace oxygine
{
    MaskedSprite::MaskedSprite() : _useRChannel(false)
    {
    }

    MaskedSprite::~MaskedSprite()
    {
    }

    void MaskedSprite::copyFrom(const MaskedSprite& src, cloneOptions opt)
    {
        _Sprite::copyFrom(src, opt);
        _useRChannel = src._useRChannel;
        //_mask = src._mask;
    }

    spSprite MaskedSprite::getMask() const
    {
        return _mask;
    }

    void MaskedSprite::setMask(spSprite mask, bool useRChannel)
    {
        _mask = mask;
        _useRChannel = useRChannel;
    }

    void MaskedSprite::render(const RenderState& parentRS)
    {
        if (_mask && _mask->getAnimFrame().getDiffuse().base)
        {
            Renderer::transform t = getGlobalTransform(_mask);
            RectF maskDest = _mask->getDestRect();
            RectF maskSrc = _mask->getSrcRect();
            //maskDest

            const Diffuse& df = _mask->getAnimFrame().getDiffuse();


            bool rchannel       = _useRChannel ? true    : (df.alpha ? true     : false);
            spNativeTexture msk = _useRChannel ? df.base : (df.alpha ? df.alpha : df.base);

            MaskedRenderer mr(msk, maskSrc, maskDest, t, rchannel);
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
        const char* id = data->node.attribute("mask").as_string(0);
        if (!id)
            return;

        spSprite mask = data->root->getDescendantT<Sprite>(id, ep_ignore_error);
        setMask(mask);
    }
}
