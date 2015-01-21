#include "VisualStyle.h"
#include "Actor.h"
#include <sstream>
#include "RenderState.h"
#include "Serialize.h"
#include "utils/stringUtils.h"

namespace oxygine
{
	VisualStyle::VisualStyle():_color(255,255,255,255), _blend(Renderer::getPremultipliedAlphaRender() ? blend_premultiplied_alpha : blend_alpha)
	{

	}

	void VStyleActor::copyFrom(const VStyleActor &src, cloneOptions opt)
	{
		_Actor::copyFrom(src, opt);
		_vstyle = src._vstyle;
	}

	void VisualStyle::_apply(const RenderState &rs)
	{	
		//rs.renderer->setMaterial(&_material);		
		rs.renderer->setBlendMode(_blend);
		//rs.renderer->setTexture()
	}

	void VisualStyle::setColor(const Color &color)
	{
		_color = color;
	}
	void VisualStyle::setBlendMode(blend_mode mode)
	{
		_blend = mode; 
	}

	std::string VisualStyle::dump() const
	{
		VisualStyle def;

		std::stringstream stream;
		if (_color != def.getColor())
		{
			stream << "color=(" << (int)_color.r << ", " << (int)_color.g << ", " << (int)_color.b << ", " << (int)_color.a << ")";
		}

		if (_blend != def.getBlendMode())
		{
			stream << "blend=" << (int)_blend;
		}
		

		return stream.str();
	}

	void VStyleActor::serialize(serializedata* data)
	{
		_Actor::serialize(data);
		if (_vstyle.getColor() != Color(0xffffffff))
			data->node.append_attribute("color").set_value(color2hex(_vstyle.getColor()).c_str());
		//if (_vstyle.getBlendMode() != )
		//data->node.append_attribute("blend").set_value(color2hex(_vstyle.getColor()).c_str());

		data->node.set_name("VStyleActor");
	}

	void VStyleActor::deserialize(const deserializedata* data)
	{
		_Actor::deserialize(data);
		setColor(hex2color(data->node.attribute("color").as_string("ffffffff")));		
	}
}
