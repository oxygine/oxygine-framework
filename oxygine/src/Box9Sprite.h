#pragma once
#include "oxygine_include.h"
#include "Sprite.h"
namespace oxygine
{
    DECLARE_SMART(Box9Sprite, spBox9Sprite);

    class Box9Sprite: public _Sprite
    {
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

        bool isOn(const Vector2& localPosition);

        void setVerticalMode(StretchMode m);
        void setHorizontalMode(StretchMode m);

        /**
        Set distance from left, right, top and bottom edges
        */
        void setGuides(float x1, float x2, float y1, float y2);
        void setVerticalGuides(float x1, float x2);
        void setHorizontalGuides(float y1, float y2);

        std::string dump(const dumpOptions&) const;

        void serialize(serializedata* data);
        void deserialize(const deserializedata* data);

        virtual RectF getDestRect() const;

    protected:
        void sizeChanged(const Vector2& size);
        void animFrameChanged(const AnimationFrame& f);

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

        virtual void doRender(const RenderState& rs);
    };
}

#ifdef OX_EDITOR
#include "EditorBox9Sprite.h"
#else
typedef oxygine::Box9Sprite BaseBox9Sprite;
#endif

