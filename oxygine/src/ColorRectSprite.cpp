#include "ColorRectSprite.h"
#include "core/Renderer.h"
#include "RenderState.h"
#include "Serialize.h"

namespace oxygine
{
	void ColorRectSprite::copyFrom(const ColorRectSprite &src, cloneOptions opt)
	{
		Sprite::copyFrom(src, opt);
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

	void ColorRectSprite::serialize(serializedata* data)
	{
		Actor::serialize(data);
		pugi::xml_node node = data->node;
		//node.append_attribute("")
		data->node.set_name("ColorRectSprite");
	}
}