#include "AnimationFrame.h"
namespace oxygine
{
	void AnimationFrame::init(ResAnim *rs, const Diffuse &df, const RectF &srcRect, const RectF &destRect, const Vector2 &frame_size)
	{
		_resAnim = rs;
		_diffuse = df;
		_srcRect = srcRect;
		_destRect = destRect;
		_frameSize = frame_size.cast<PointS>();
	}

	AnimationFrame AnimationFrame::clip(const RectF &rect) const
	{
		AnimationFrame f = *this;
		float w = (float)_diffuse.base->getWidth();
		float h = (float)_diffuse.base->getHeight();

		f._destRect.clip(rect);

		RectF srcRect = _srcRect * Vector2(w, h);

		float sc = 1.0f;//(float)srcRect.getWidth() / f._destRect.getWidth();


		f._srcRect.pos = srcRect.pos - (_destRect.pos - f._destRect.pos) * sc;
		f._srcRect.size = srcRect.size - (_destRect.size - f._destRect.size) * sc;
		f._srcRect = f._srcRect / Vector2(w, h);

		f._frameSize = rect.size;
		f._destRect.pos -= rect.pos;

		return f;
	}

	AnimationFrame AnimationFrame::flip(bool vertical, bool horizontal) const
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
}