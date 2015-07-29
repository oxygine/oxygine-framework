#pragma once
#include "oxygine_include.h"
#include "Sprite.h"

namespace oxygine
{
    DECLARE_SMART(MaskedSprite, spMaskedSprite);
    class MaskedSprite: public _Sprite
    {
    public:
        DECLARE_COPYCLONE_NEW(MaskedSprite);

        MaskedSprite();
        ~MaskedSprite();

        spSprite getMask() const;

        void setMask(spSprite, bool useRChannel = false);

        void serialize(serializedata* data);
        void deserialize(const deserializedata* data);
        void deserializeLink(const deserializeLinkData*);

    protected:
        void render(const RenderState& parentRS);

    private:
        spSprite _mask;
        bool _useRChannel;
    };
}

#ifdef OX_EDITOR
#include "EditorMaskedSprite.h"
#else
namespace oxygine
{
    typedef MaskedSprite _MaskedSprite;
}
#endif
