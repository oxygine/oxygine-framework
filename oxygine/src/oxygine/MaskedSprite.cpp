#include "MaskedSprite.h"
#include "RenderState.h"
#include "MaskedRenderer.h"
#include "Serialize.h"
#include "Material.h"

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
        inherited::copyFrom(src, opt);
        _useRChannel = src._useRChannel;
        //_mask = src._mask;
    }

    spSprite MaskedSprite::getMask() const
    {
        return _mask;
    }

    bool    MaskedSprite::getUseRChannel() const
    {
        return _useRChannel;
    }

    void MaskedSprite::setMask(spSprite mask, bool useRChannel)
    {
        _mask = mask;
        _useRChannel = useRChannel;
    }


    void MaskedSprite::render(const RenderState& parentRS)
    {
        _rdelegate->render(this, parentRS);
    }


    void MaskedSprite::serialize(serializedata* data)
    {
        inherited::serialize(data);
        data->node.set_name("MaskedSprite");
    }

    void MaskedSprite::deserialize(const deserializedata* data)
    {
        inherited::deserialize(data);
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
