#pragma once
#include "oxygine_include.h"
#include "Sprite.h"

namespace oxygine
{
	DECLARE_SMART(ColorRectSprite, spColorRectSprite);

    class ColorRectSprite: public _Sprite
	{
	public:
		DECLARE_COPYCLONE_NEW(ColorRectSprite);
		ColorRectSprite();
		~ColorRectSprite();

		void serialize(serializedata* data);
        void deserialize(const deserializedata *data);

		void doRender(const RenderState &rs);

	private:
	};
}

#ifdef OX_EDITOR
#include "EditorColorRectSprite.h"
#else
namespace oxygine
{
    typedef ColorRectSprite _ColorRectSprite;
}
#endif
