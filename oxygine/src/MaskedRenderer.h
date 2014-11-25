#pragma once
#include "oxygine_include.h"
#include "STDRenderer.h"

namespace oxygine
{
	class MaskedRenderer : public STDRenderer
	{
	public:
		MaskedRenderer(spNativeTexture mask, const RectF &srcRect, const RectF &destRect, const transform &t, bool channelR);

		//void draw(const RState *rs, const RectF &destRect) OVERRIDE;

	protected:
		void preDrawBatch();
		//void addVertices(const void *data, unsigned int size, const VertexDeclaration *decl) OVERRIDE;

		spNativeTexture _mask;
		RectF _clipMask;
		ClipUV _clipUV;
	};
}
