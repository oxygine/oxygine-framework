#pragma once
#include "oxygine_include.h"
#include "core/Texture.h"
#include "math/Rect.h"
#include "res/ResAnim.h"
#include "Actor.h"
#include "AnimationFrame.h"
#include "VisualStyle.h"
#include "TweenAnim.h"

namespace oxygine
{
    class ResAnim;

    DECLARE_SMART(Sprite, spSprite);
    class Sprite : public VStyleActor
    {
        INHERITED(VStyleActor);
    public:
        DECLARE_COPYCLONE_NEW(Sprite);

        Sprite();
        ~Sprite();

        const AnimationFrame&   getAnimFrame() const {return _frame;}
        RectF                   getDestRect() const OVERRIDE;
        const Diffuse&          getDiffuse() const {return _frame.getDiffuse();}
        bool                    getManageResAnim() const {return (_flags & flag_manageResAnim) != 0;}
        const RectF&            getSrcRect() const {return _frame.getSrcRect();}
        const ResAnim*          getResAnim() const {return _frame.getResAnim();}
        int                     getColumn() const {return _frame.getColumn();}
        int                     getRow() const {return _frame.getRow();}

        /**load/unload atlas automatically or not*/
        void                    setManageResAnim(bool manage);
        /**Changes sprite image*/
        void                    setAnimFrame(const AnimationFrame& f);
        /**Takes AnimationFrame from ResAnim and set it as current to Sprite. Shows assert is resanim is null. Using this method is more safe than 'setAnimFrame(const AnimationFrame &f)'*/
        void                    setAnimFrame(const ResAnim* resanim, int col = 0, int row = 0);
        virtual void            setResAnim(const ResAnim* resanim, int col = 0, int row = 0);
        void                    setRow(int row);
        void                    setColumn(int column);
        void                    setColumnRow(int column, int row);

        bool                    isOn(const Vector2& localPosition);

        bool                    isFlippedX() const {return (_flags & flag_flipX) != 0;}
        bool                    isFlippedY() const {return (_flags & flag_flipY) != 0;}
        void                    setFlippedX(bool flippedX);
        void                    setFlippedY(bool flippedY);
        void                    setFlipped(bool flippedX, bool flippedY);

        void serialize(serializedata* data);
        void deserialize(const deserializedata* data);

        typedef oxygine::TweenAnim TweenAnim;
        std::string dump(const dumpOptions&) const;

        void doRender(const RenderState& rs);
    protected:
        enum
        {
            flag_manageResAnim = flag_last << 1,
            flag_flipX = flag_last << 2,
            flag_flipY = flag_last << 3
        };
        virtual void changeAnimFrame(const AnimationFrame& f);
        virtual void animFrameChanged(const AnimationFrame& f);
        void sizeChanged(const Vector2& size) OVERRIDE;

        Vector2 _localScale;
        AnimationFrame _frame;
    };
}

EDITOR_INCLUDE(Sprite);