#include "AnimationFrame.h"
#include "res/ResAnim.h"
namespace oxygine
{
    void AnimationFrame::init(ResAnim* rs, const Diffuse& df, const RectF& srcRect, const RectF& destRect, const Vector2& frame_size)
    {
        _resAnim = rs;
        _diffuse = df;
        _srcRect = srcRect;
        _destRect = destRect;
        _frameSize = frame_size.cast<PointS>();
    }

    void AnimationFrame::init2(ResAnim* rs, short col, short row, const Diffuse& df, const RectF& srcRect, const RectF& destRect, const Vector2& frame_size)
    {
        _column = col;
        _row = row;
        _resAnim = rs;
        _diffuse = df;
        _srcRect = srcRect;
        _destRect = destRect;
        _frameSize = frame_size.cast<PointS>();
    }

    AnimationFrame AnimationFrame::getClipped(const RectF& rect) const
    {
        AnimationFrame f = *this;
        float w = (float)_diffuse.base->getWidth();
        float h = (float)_diffuse.base->getHeight();

        f._destRect.clip(rect);
        if (f._destRect.isEmpty())
            f._destRect = RectF(0, 0, 0, 0);


        RectF srcRect = _srcRect * Vector2(w, h);

        float sc = 1.0f;//(float)srcRect.getWidth() / f._destRect.getWidth();
        if (_resAnim)
            sc = _resAnim->getScaleFactor();


        f._srcRect.pos = srcRect.pos - (_destRect.pos - f._destRect.pos) * sc;
        f._srcRect.size = srcRect.size - (_destRect.size - f._destRect.size) * sc;
        f._srcRect = f._srcRect / Vector2(w, h);

        f._frameSize = rect.size;
        f._destRect.pos -= rect.pos;

        return f;
    }

    AnimationFrame AnimationFrame::getFlipped(bool vertical, bool horizontal) const
    {
        AnimationFrame f = *this;
        if (vertical)
        {
            f._srcRect.setY(_srcRect.getBottom());
            f._srcRect.setHeight(-_srcRect.getHeight());
        }

        if (horizontal)
        {
            f._srcRect.setX(_srcRect.getRight());
            f._srcRect.setWidth(-_srcRect.getWidth());
        }

        return f;
    }

    void AnimationFrame::flipX()
    {
        _srcRect.setX(_srcRect.getRight());
        _srcRect.setWidth(-_srcRect.getWidth());
    }

    void AnimationFrame::flipY()
    {
        _srcRect.setY(_srcRect.getBottom());
        _srcRect.setHeight(-_srcRect.getHeight());
    }

    AnimationFrame::AnimationFrame(spNativeTexture t)
    {
        _row = _column = 0;
        _resAnim = 0;
        _diffuse.base = t;
        //_diffuse.premultiplied = true;
        _srcRect = RectF(0, 0, 1, 1);
        _destRect = RectF(0, 0, (float)t->getWidth(), (float)t->getHeight());
        _frameSize = Vector2((float)t->getWidth(), (float)t->getHeight());
    }
}
