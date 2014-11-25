#include "ColorRectSprite.h"
#include "core/Renderer.h"
#include "RenderState.h"
#include "STDRenderer.h"
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
		
		
		rs.renderer->setTexture(Renderer::white, 0);		
		rs.renderer->draw(&rs, getColor(), RectF(0, 0, 1, 1), getDestRect());
	}

	void ColorRectSprite::serialize(serializedata* data)
	{
		Actor::serialize(data);
		pugi::xml_node node = data->node;
		//node.append_attribute("")
		data->node.set_name("ColorRectSprite");
	}
}