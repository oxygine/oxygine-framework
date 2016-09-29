#pragma once
#include "oxygine_include.h"
#include "math/Rect.h"
#include "core/Texture.h"
#include "core/NativeTexture.h"

namespace oxygine
{
    DECLARE_SMART(Texture, spTexture);
    DECLARE_SMART(NativeTexture, spNativeTexture);

    class Diffuse
    {
    public:
        Diffuse(): premultiplied(true) {}
        spNativeTexture base;
        spNativeTexture alpha;
        bool premultiplied;
    };

    class HitTestData
    {
    public:
        HitTestData(): data(0), w(0), h(0), pitch(0) {}

        const unsigned char* data;
        short w, h;
        unsigned char pitch;
    };

    class ResAnim;
    class AnimationFrame
    {
    public:
        AnimationFrame() : _srcRect(0, 0, 1, 1), _destRect(0, 0, 1, 1), _resAnim(0), _row(0), _column(0) {}
        AnimationFrame(spNativeTexture t);

        void init(ResAnim* rs, const Diffuse& df,
                  const RectF& srcRect, const RectF& destRect, const Vector2& frame_size);
        void init2(ResAnim* rs, short column, short row, const Diffuse& df,
                   const RectF& srcRect, const RectF& destRect, const Vector2& frame_size);
        /**ResAnim should be valid!*/
        AnimationFrame getClipped(const RectF& rect) const;
        AnimationFrame getFlipped(bool vertical, bool horizontal) const;

        /**Returns size of frame in pixels*/
        const Vector2&  getSize() const {return _frameSize;}
        float           getWidth() const {return _frameSize.x;}
        float           getHeight() const {return _frameSize.y;}

        ResAnim*        getResAnim() const {return _resAnim;}
        short           getColumn() const {return _column;}
        short           getRow() const {return _row;}
        const RectF&    getSrcRect() const {return _srcRect;}
        const RectF&    getDestRect() const {return _destRect;}
        const Diffuse&  getDiffuse() const {return _diffuse;}
        const HitTestData& getHitTestData()const { return _hittest; }

        void            setSrcRect(const RectF& r) {_srcRect = r;}
        void            setDestRect(const RectF& r) {_destRect = r;}
        void            setResAnim(ResAnim* rs) {_resAnim = rs;}
        void            setDiffuse(const Diffuse& d) { _diffuse = d; }
        void            setSize(const Vector2& size) {_frameSize = size;}
        void            setHitTestData(const HitTestData& ad) { _hittest = ad; }
        void            setRow(int v) {_row = v;}
        void            setColumn(int v) {_column = v;}

        void            flipX();
        void            flipY();

    private:
        enum flags
        {
            clipped = 0x01,
        };

        Diffuse         _diffuse;

        RectF           _srcRect;
        RectF           _destRect;
        Vector2         _frameSize;//real size without clipping
        ResAnim*        _resAnim;
        short           _row;
        short           _column;

        HitTestData       _hittest;
        //unsigned short    _flags;
    };
}
