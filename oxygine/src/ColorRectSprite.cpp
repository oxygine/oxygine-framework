#include "ColorRectSprite.h"
#include "core/Renderer.h"
#include "RenderState.h"

namespace oxygine
{
	ColorRectSprite::ColorRectSprite(const ColorRectSprite &src, cloneOptions opt):Sprite(src, opt)
	{

	}

	ColorRectSprite::ColorRectSprite()
	{

	}

	ColorRectSprite::~ColorRectSprite()
	{

	}

	void ColorRectSprite::doRender(const RenderState &rs)
	{
		_vstyle._apply(rs);
		Diffuse df;
		df.base = Renderer::white;
		rs.renderer->setDiffuse(df);
		RectF srcRect = getSrcRect();		
		RectF destRect = getDestRect();
		rs.renderer->draw(srcRect, destRect);
	}
}