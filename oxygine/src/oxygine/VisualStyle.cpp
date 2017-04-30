#include "VisualStyle.h"
#include "actor/Actor.h"
#include <sstream>
#include "RenderState.h"
#include "Serialize.h"
#include "utils/stringUtils.h"
#include "MaterialCache.h"

namespace oxygine
{
    VisualStyle::VisualStyle(): _color(Color::White), _blend(blend_premultiplied_alpha)
    {

    }

    VStyleActor::VStyleActor()
    {
        STDMaterialX mat;
        mat._blend = blend_premultiplied_alpha;
        mat._base = STDRenderer::white;

        _mat = mc().cache(mat);
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
            stream << "color=" << color2hex(_color);
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

    void VStyleActor::setBlendMode(blend_mode mode)
    {
        if (getBlendMode() == mode)
            return;

        _vstyle.setBlendMode(mode);

        _mat = _mat->clone();
        _mat->_blend = mode;
        _mat = mc().cache(*_mat.get());
        matChanged();
    }

    void VStyleActor::setMat(spSTDMaterialX mat)
    {
        //if (_mat == mat)
        //    return;
        _mat = mat;
        matChanged();
    }
}
