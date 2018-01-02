#pragma once
#include "oxygine-include.h"
#include "Material.h"
#include "actor/Actor.h"

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
        const Color&            getAddColor() const;

        void                    setColor(const Color& color);
        void                    setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
        void                    setBlendMode(blend_mode mode);
        void                    setAddColor(const Color& color);
        void                    setAddColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0);

        typedef Property<Color, const Color&, VStyleActor, &VStyleActor::getColor, &VStyleActor::setColor> TweenColor;
        typedef Property<Color, const Color&, VStyleActor, &VStyleActor::getAddColor, &VStyleActor::setAddColor> TweenAddColor;

        bool getBounds(RectF& b) const  override { b = getDestRect();  return true; }

        void                    setMat(spSTDMaterial mat);
        spSTDMaterial _mat;

    protected:
        virtual void matChanged() {}
        VisualStyle _vstyle;


    };

}

EDITOR_INCLUDE(VStyleActor);