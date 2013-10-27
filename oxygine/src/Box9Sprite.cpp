#include "Box9Sprite.h"
#include "res/ResAnim.h"
#include "math/ScalarMath.h"
#include "RenderState.h"

namespace oxygine
{
	Box9Sprite::Box9Sprite(const Box9Sprite &src, cloneOptions opt):Sprite(src, opt)
	{
		_prepared = src._prepared;

		_vertMode = src._vertMode;
		_horzMode = src._horzMode;

		_guideX[0] = src._guideX[0];
		_guideX[1] = src._guideX[1];
		_guideY[0] = src._guideY[0];
		_guideY[1] = src._guideY[1];

		_guidesX = src._guidesX;
		_guidesY = src._guidesY;
		_pointsX = src._pointsX;
		_pointsY = src._pointsY;
	}

	Box9Sprite::Box9Sprite() :
        _prepared(false),
		_vertMode(STRETCHING),
		_horzMode(STRETCHING)
		
	{
		_guideX[0] = 0.0f;
		_guideX[1] = 0.0f;
		_guideY[0] = 0.0f;
		_guideY[1] = 0.0f;
	}

	void Box9Sprite::setVerticalMode(StretchMode m)
	{
		_vertMode = m;
		_prepared = false;
	}

	void Box9Sprite::setHorizontalMode(StretchMode m)
	{
		_horzMode = m;
		_prepared = false;
	}

	void Box9Sprite::setGuides(float x1, float x2, float y1, float y2)
	{
		_guideX[0] = x1;
		_guideX[1] = x2;
		_guideY[0] = y1;
		_guideY[1] = y2;
		_prepared = false;
	}

	void Box9Sprite::setVerticalGuides(float x1, float x2)
	{
		_guideX[0] = x1;
		_guideX[1] = x2;
		_prepared = false;
	}

	void Box9Sprite::setHorizontalGuides(float y1, float y2)
	{
		_guideY[0] = y1;
		_guideY[1] = y2;
		_prepared = false;
	}

	void Box9Sprite::animFrameChanged(const AnimationFrame &f)
	{
		_prepared = false;

		ResAnim *resanim = f.getResAnim();
		if (resanim)
		{
			float scaleFactor = resanim->getScaleFactor();

			pugi::xml_attribute attr = resanim->getAttribute("guideX1");
			if (!attr.empty())
				_guideX[0] = attr.as_float() * scaleFactor;

			attr = resanim->getAttribute("guideX2");
			if (!attr.empty())
				_guideX[1] = attr.as_float() * scaleFactor;

			attr = resanim->getAttribute("guideY1");
			if (!attr.empty())
				_guideY[0] = attr.as_float() * scaleFactor;

			attr = resanim->getAttribute("guideY2");
			if (!attr.empty())
				_guideY[1] = attr.as_float() * scaleFactor;

			attr = resanim->getAttribute("vertical");
			if (!attr.empty())
				_vertMode = (StretchMode)attr.as_uint();

			attr = resanim->getAttribute("horizontal");
			if (!attr.empty())
				_horzMode = (StretchMode)attr.as_uint();
		}
	}

	RectF Box9Sprite::getDestRect() const
	{
		//return Sprite::getDestRect();
		return Actor::getDestRect();
	}

	void Box9Sprite::prepare()
	{
		_guidesX.resize(4);
		_guidesY.resize(4);
		_pointsX.resize(0);
		_pointsY.resize(0);

		float fFrameWidth = (float)_frame.getFrameSize().x;
		float fFrameHeight = (float)_frame.getFrameSize().y;
		
		/*
		float fActorWidth = max((float)getSize().x, fFrameWidth);
		float fActorHeight = max((float)getSize().y, fFrameHeight);
		*/
		
		float fActorWidth = getSize().x;
		float fActorHeight = getSize().y;
		
		if (_guideX[1] == 0.0f)
			_guideX[1] = fFrameWidth;
		if (_guideY[1] == 0.0f)
			_guideY[1] = fFrameHeight;

		RectF srcFrameRect = _frame.getSrcRect();
	
		_guidesX[0] = srcFrameRect.getLeft(); // these guides contains floats from 0.0 to 1.0, compared to original guides which contain floats in px
		_guidesX[1] = interpolate(srcFrameRect.getLeft(), srcFrameRect.getRight(), _guideX[0] / fFrameWidth); // lerp is needed here cuz the frame might be in an atlas
		_guidesX[2] = interpolate(srcFrameRect.getLeft(), srcFrameRect.getRight(), _guideX[1] / fFrameWidth);
		_guidesX[3] = srcFrameRect.getRight();

		_guidesY[0] = srcFrameRect.getTop();
		_guidesY[1] = interpolate(srcFrameRect.getTop(), srcFrameRect.getBottom(), _guideY[0] / fFrameHeight);
		_guidesY[2] = interpolate(srcFrameRect.getTop(), srcFrameRect.getBottom(), _guideY[1] / fFrameHeight);
		_guidesY[3] = srcFrameRect.getBottom();

		// filling X axis
		_pointsX.push_back(0.0f);
		_pointsX.push_back(_guideX[0]);

		if (_horzMode == STRETCHING)
		{
			_pointsX.push_back(fActorWidth - (fFrameWidth - _guideX[1]));
			_pointsX.push_back(fActorWidth);
		}
		else if (_horzMode == TILING || _horzMode == TILING_FULL)
		{
			float curX = _guideX[0];
			float rightB = fActorWidth - (fFrameWidth - _guideX[1]); // right bound (in px)
			float centerPart = _guideX[1] - _guideX[0]; // length of the center piece (in px)

			// now we add a new center piece every time until we reach right bound
			while(1)
			{
				curX += centerPart;

				if (curX <= rightB)
				{
					_pointsX.push_back(curX);
				}
				else
				{
					if (_horzMode == TILING_FULL)
					{
						_pointsX.push_back(rightB);
						_pointsX.push_back(fActorWidth);
					}
					else
						_pointsX.push_back(curX - centerPart + (fFrameWidth - _guideX[1]));
					break;
				}
			}
		}

		// filling Y axis
		_pointsY.push_back(0.0f);
		_pointsY.push_back(_guideY[0]);

		if (_vertMode == STRETCHING)
		{
			_pointsY.push_back(fActorHeight - (fFrameHeight - _guideY[1]));
			_pointsY.push_back(fActorHeight);
		}
		else if (_vertMode == TILING || _vertMode == TILING_FULL)
		{
			float curY = _guideY[0];
			float bottomB = fActorHeight - (fFrameHeight - _guideY[1]); // bottom bound (in px)
			float centerPart = _guideY[1] - _guideY[0]; // length of the center piece (in px)

			// now we add a new center piece every time until we reach right bound
			while(1)
			{
				curY += centerPart;

				if (curY <= bottomB)
				{
					_pointsY.push_back(curY);
				}
				else
				{
					if (_vertMode == TILING_FULL)
					{
						_pointsY.push_back(bottomB);
						_pointsY.push_back(fActorHeight);
					}
					else
						_pointsY.push_back(curY - centerPart + (fFrameHeight - _guideY[1]));
					break;
				}
			}
		}

		_prepared = true;
	}

	void Box9Sprite::sizeChanged(const Vector2 &size)
	{
		_prepared = false;
	}

	void Box9Sprite::doRender(const RenderState &rs)
	{
		if (!_prepared)
			prepare();

		_vstyle._apply(rs);
		const Diffuse &df = _frame.getDiffuse();
		if (df.base)
		{
			if (_guidesX.size() >= 2 || _guidesY.size() >= 2)
			{
				rs.renderer->setDiffuse(df);

				// number of vertical blocks
				int vc = _pointsX.size() - 1;
				// number of horizontal blocks
				int hc = _pointsY.size() - 1;
			
				int xgi = 0; // x guide index
				int ygi = 0;
				for (int yc = 0; yc < hc; yc++)
				{
					for (int xc = 0; xc < vc; xc++)
					{
						if (xc == 0) // select correct index for _guides% arrays
							xgi = 0;
						else if (xc == _pointsX.size() - 2)
							xgi = 2;
						else
							xgi = 1;
					
						if (yc == 0)
							ygi = 0;
						else if (yc == _pointsY.size() - 2)
							ygi = 2;
						else
							ygi = 1;

						RectF srcRect(_guidesX[xgi], _guidesY[ygi], _guidesX[xgi+1] - _guidesX[xgi], _guidesY[ygi+1] - _guidesY[ygi]);
						RectF destRect(_pointsX[xc], _pointsY[yc], _pointsX[xc+1] - _pointsX[xc], _pointsY[yc+1] - _pointsY[yc]);

						rs.renderer->draw(srcRect, destRect);
					}
				}
			}
		}
	}
}