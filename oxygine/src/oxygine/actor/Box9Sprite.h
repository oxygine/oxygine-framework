#pragma once
#include "../oxygine-include.h"
#include "Sprite.h"

namespace oxygine
{
    DECLARE_SMART(Box9Sprite, spBox9Sprite);

    class Box9Sprite: public Sprite
    {
        INHERITED(Sprite);
    public:
        DECLARE_COPYCLONE_NEW(Box9Sprite);
        enum StretchMode
        {
            TILING,
            TILING_FULL,
            STRETCHING
        };

        Box9Sprite();

        StretchMode getVerticalMode() const {return _vertMode;}
        StretchMode getHorizontalMode() const {return _horzMode;}

        void setVerticalMode(StretchMode m);
        void setHorizontalMode(StretchMode m);

        /**
        Set distance from left, right, top and bottom edges
        */
        void setGuides(float x1, float x2, float y1, float y2);
        void setVerticalGuides(float x1, float x2);
        void setHorizontalGuides(float y1, float y2);

        bool isOn(const Vector2& localPosition, float localScale) override;

        std::string dump(const dumpOptions&) const override;

        void serialize(serializedata* data) override;
        void deserialize(const deserializedata* data) override;

        RectF getDestRect() const override;

    protected:
        void sizeChanged(const Vector2& size)  override;
        void animFrameChanged(const AnimationFrame& f) override;
        void changeAnimFrame(const AnimationFrame& f) override;

        bool _prepared;

        StretchMode _vertMode;
        StretchMode _horzMode;

        float _guideX[2];
        float _guideY[2];

        std::vector<float> _guidesX;
        std::vector<float> _guidesY;
        std::vector<float> _pointsX;
        std::vector<float> _pointsY;

        void prepare();

        void doRender(const RenderState&) override;
    };
}


EDITOR_INCLUDE(Box9Sprite);