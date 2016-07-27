#pragma once
#include "oxygine_include.h"
#include "Actor.h"

namespace oxygine
{
    class RenderState;
    class VisualStyle
    {
    public:

        VisualStyle();

        const Color&        getColor() const {return _color;}
        blend_mode          getBlendMode() const {return _blend;}

        void setColor(const Color& color);
        void setBlendMode(blend_mode mode);

        std::string dump() const;



    protected:
        Color _color;
        blend_mode _blend;
    };

    DECLARE_SMART(VStyleActor, spVStyleActor);
    class VStyleActor : public _Actor
    {
    public:
        DECLARE_COPYCLONE_NEW(VStyleActor);
        VStyleActor();

        void serialize(serializedata* data);
        void deserialize(const deserializedata* data);

        blend_mode              getBlendMode() const {return _vstyle.getBlendMode();}
        const Color&            getColor() const;

        void                    setColor(const Color& color);
        void                    setBlendMode(blend_mode mode) {_vstyle.setBlendMode(mode);}

        typedef Property<Color, const Color&, VStyleActor, &VStyleActor::getColor, &VStyleActor::setColor> TweenColor;

		bool getBounds(RectF& b) const  OVERRIDE { b = getDestRect();  return true; }

    protected:        
        VisualStyle _vstyle;
    };

}

#ifdef OX_EDITOR
#include "EditorVisualStyle.h"
#else
namespace oxygine
{
    typedef VStyleActor _VStyleActor;
}
#endif
