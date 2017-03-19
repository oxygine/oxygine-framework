#pragma once
#include "oxygine-include.h"
#include "Actor.h"
#include "MaterialX.h"
namespace oxygine
{
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
    class VStyleActor : public Actor
    {
        INHERITED(Actor);
    public:
        DECLARE_COPYCLONE_NEW(VStyleActor);
        VStyleActor();

        void serialize(serializedata* data) override;
        void deserialize(const deserializedata* data) override;

        blend_mode              getBlendMode() const {return _vstyle.getBlendMode();}
        const Color&            getColor() const;

        void                    setColor(const Color& color);
        void                    setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void                    setBlendMode(blend_mode mode);

        typedef Property<Color, const Color&, VStyleActor, &VStyleActor::getColor, &VStyleActor::setColor> TweenColor;

        bool getBounds(RectF& b) const  override { b = getDestRect();  return true; }

        spSTDMaterialX _mat;

    protected:
        virtual void blendModeChanged(blend_mode) {}
        VisualStyle _vstyle;


    };

}

EDITOR_INCLUDE(VStyleActor);