#include "ColorRectSprite.h"
#include "../Material.h"
#include "../MaterialCache.h"
#include "../RenderState.h"
#include "../Serialize.h"
#include "../utils/stringUtils.h"

namespace oxygine
{
    void ColorRectSprite::copyFrom(const ColorRectSprite& src, cloneOptions opt)
    {
        inherited::copyFrom(src, opt);
    }

    ColorRectSprite::ColorRectSprite()
    {
        STDMaterialX mat;
        mat._base = STDRenderer::white;

        _mat = mc().cache(mat);
    }

    ColorRectSprite::~ColorRectSprite()
    {

    }

    void ColorRectSprite::doRender(const RenderState& rs)
    {
        _rdelegate->doRender(this, rs);
    }

    void ColorRectSprite::sizeChanged(const Vector2& size)
    {
        Actor::sizeChanged(size);
    }

    void ColorRectSprite::serialize(serializedata* data)
    {
        VStyleActor::serialize(data);
        pugi::xml_node node = data->node;
        //node.append_attribute("")
        node.set_name("ColorRectSprite");
        //node.append_attribute("color").set_value(color2hex(_color));
    }

    void ColorRectSprite::deserialize(const deserializedata* data)
    {
        VStyleActor::deserialize(data);
    }
}
