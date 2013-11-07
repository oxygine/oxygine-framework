#include "SlidingActor.h"
#include "ClipRectActor.h"
//#include "DragHandler.h"
#include "PointerState.h"
#include "UpdateState.h"
#include "Draggable.h"
#include "initActor.h"

namespace oxygine
{
	float _defaultTouchThreshold = 15;
	void SlidingActor::setDefaultTouchThreshold(float val)
	{
		_defaultTouchThreshold = val;
	}

	SlidingActor::SlidingActor():_prevPos(0, 0), _speed(0, 0), _sliding(false), _downPos(0, 0), _rad(_defaultTouchThreshold), _downTime(0), _down(false), _maxSpeed(250), _snapSize(0, 0), 
		_movingX(false), _movingY(false), _snapSpeed(1.0f)
	{
		_clip = initActor(new ClipRectActor, 
			arg_attachTo = this);

		_clip->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &SlidingActor::_newEvent));
		_clip->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &SlidingActor::_newEvent));
		_clip->addEventListener(TouchEvent::MOVE, CLOSURE(this, &SlidingActor::_newEvent));
	}
	
	void SlidingActor::destroy()
	{
		_clip->removeChildren();

		_drag.destroy();
		_content = 0; 
	}

	SlidingActor::~SlidingActor()
	{
		destroy();		
	}

	void SlidingActor::setTouchThreshold(float rad)
	{
		_rad = rad;
	}

	void SlidingActor::sizeChanged(const Vector2 &size)
	{
		if (_clip)
			_clip->setSize(size);

		updateDragBounds();
	}

	void SlidingActor::snap()
	{
		updateDragBounds();
		_drag.snapClient2Bounds();
	}

	void SlidingActor::setSnapPageSize(const Vector2 &size)
	{
		_snapSize = size;
	}

	void SlidingActor::setContent(spActor content)
	{
		if (_content)
		{
			_drag.destroy();
			_content->detach();
		}

		_content = content;
		_drag.init(content.get());
		
		_clip->addChild(_content);

		updateDragBounds();
	}

	void SlidingActor::updateDragBounds()
	{
		if (!_content)
			return;
		float w = _content->getWidth() - _clip->getWidth();
		float h = _content->getHeight() - _clip->getHeight();
		RectF bounds(-w, -h, w, h);		

		_drag.setDragBounds(bounds);
	}

	float getOffset(float cp, float size)
	{
		float half = size/2;

		float pos = cp - half;
		pos = int(pos / size) * size;
		float d = cp - pos;

		//float t = 
		d = scalar::clamp(d, -10.0f, 10.0f);

		return d;
	}

	void SlidingActor::doUpdate(const UpdateState &us)
	{
		if (!_content)
			return;

		float  dt = us.dt/1000.0f;
		Vector2 contentPos = _content->getPosition();
		
		if (_holded && (getTimeMS() > _downTime + 100))
		{
			Vector2 dist = contentPos - _prevPos;
			if (dist.sqlength() < 1)
				_speed = Vector2(0, 0);
		}

		if (!_down)
		{
			dt *= 20 * _snapSpeed;
			bool callSnap = false;

			if (_snapSize.x >= 0.999 && _movingX)
			{
				float d = getOffset(_content->getX(), (float)_snapSize.x) * dt;
				_content->setX(_content->getX() - d);

				if (d == 0)
				{
					_movingX = false;					
					callSnap = true;
				}
			}


			if (_snapSize.y && _movingY)
			{
				float d = getOffset(_content->getY(), (float)_snapSize.y) * dt;
				_content->setY(_content->getY() - d);
				if (d == 0)
				{
					_movingY = false;
					callSnap = true;
				}
			}
			

			if (callSnap && _cbSlideDone)
				_cbSlideDone(0);
		}
			
		if (!_sliding)
			return;

		if (_speed.length() < 0.001)
		{
			_sliding = false;
			
			return;
		}

		
		
		Vector2 newPos = contentPos + _speed * dt;
		_content->setPosition(newPos);
		//_slideOnTopDH->snapClient2Bounds();

		if (!(newPos == _content->getPosition()))
		{
			_sliding = false;
		}

		Vector2 s = -_speed;
		s.normalizeTo(200);
		_speed = _speed + s * dt;

		if (_speed.dot(s) > 0.9)
		{
			_sliding = false;			
		}

		if (!_sliding)
		{
			//slideDone();
		}
	}

	void SlidingActor::handleEvent(Event *event)
	{
		bool touchEvent = event->type > _et_TouchFirst && event->type < _et_TouchLast;
		if (touchEvent && _holded)
		{

		//	return;
		}
		
		Actor::handleEvent(event);
	}

	void SlidingActor::_newEvent(Event *event)
	{
		TouchEvent *te = safeCast<TouchEvent*>(event);
		switch(te->type)
		{
		case TouchEvent::TOUCH_DOWN:
			_holded = event->target;
			_downPos = te->localPosition;
			_down = true;
			break;

		case TouchEvent::TOUCH_UP:
			{		
				_movingY = _movingX = true;

				_holded = 0;
				_down = false;

				if (_snapSize.sqlength() < 0.000001 && _cbSlideDone)
					_cbSlideDone(0);
			}			
			break;

		case TouchEvent::MOVE:
			{
				Vector2 offset = _downPos - te->localPosition;
				float d = offset.dot(offset);
				if (_holded && (d >= _rad * _rad))
				{
					TouchEvent ev(TouchEvent::TOUCH_UP, false, Vector2(-10000, -10000));
					spActor act = safeSpCast<Actor>(_holded);
					while(act && act.get() != _content.get())
					{
						act->setPressed(0);
						//act->setOvered(0);
						act = act->getParent();
					}

					_content->setPressed(te->index);
					_holded = 0;
				}			
			}			
			break;
		}
	}
}
