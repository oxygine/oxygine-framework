#include "VisualStyle.h"
#include "Actor.h"
#include <sstream>
#include "RenderState.h"
#include "Serialize.h"
#include "utils/stringUtils.h"

namespace oxygine
{
    VisualStyle::VisualStyle(): _color(255, 255, 255, 255), _blend(blend_premultiplied_alpha)
    {

    }

    VStyleActor::VStyleActor()
    {
    }

    void VStyleActor::copyFrom(const VStyleActor& src, cloneOptions opt)
    {
        inherited::copyFrom(src, opt);
        _vstyle = src._vstyle;
    }

    void VisualStyle::setColor(const Color& color)
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
        inherited::serialize(data);
        if (_vstyle.getColor() != Color(0xffffffff))
            data->node.append_attribute("color").set_value(color2hex(_vstyle.getColor()).c_str());
        if (_vstyle.getBlendMode() != blend_premultiplied_alpha)
            data->node.append_attribute("blend").set_value(_vstyle.getBlendMode());

        data->node.set_name("VStyleActor");
    }

    void VStyleActor::deserialize(const deserializedata* data)
    {
        inherited::deserialize(data);
        setColor(hex2color(data->node.attribute("color").as_string("ffffffff")));
        setBlendMode((blend_mode)(data->node.attribute("blend").as_int(blend_premultiplied_alpha)));
    }

    const Color& VStyleActor::getColor() const
    {
        return _vstyle.getColor();
    }

    void VStyleActor::setColor(const Color& color)
    {
        _vstyle.setColor(color);
    }

    void VStyleActor::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        setColor(Color(r, g, b, a));
    }

}
