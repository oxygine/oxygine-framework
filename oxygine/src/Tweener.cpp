#include "Tweener.h"
#include "Actor.h"
#include "ColorRectSprite.h"
#include <limits.h>
namespace oxygine
{
	Tween::Tween():_duration(0), 
		_loops(0), 
		_percent(0), 
		_status(status_not_started),
		_elapsed(0), _twoSides(false), _ease(ease_linear), _detach(false), _delay(0), _client(0)
	{

	}

	Tween::~Tween()
	{
	}

	void Tween::reset()
	{
		_elapsed = 0;
		_status = status_not_started;
	}

	void Tween::init(timeMS duration, int loops, bool twoSides, timeMS delay, EASE ease)
	{
		_duration = duration;
		_ease = ease;
		_loops = loops;
		_twoSides = twoSides;
		_delay = delay;
				
		if (_duration <= 0)
		{
			OX_ASSERT(!"Tweener duration should be more than ZERO");
			_duration = 1;
		}
	}

	void Tween::setDoneCallback(EventCallback cb)
	{
		_cbDone = cb;
	}

	void Tween::addDoneCallback(EventCallback cb)
	{
		addEventListener(TweenEvent::DONE, cb);
	}

	float Tween::calcEase(EASE ease, float v)
	{
		float vi;
		float r = 0;
		const float s = 1.70158f;

		switch(ease)
		{
		case ease_linear:
			r = v;
			break;
		case ease_inExpo:
			r = powf(2.0f, 10.0f * (v - 1.0f));
			break;
		case ease_outExpo:
			r = 1.0f - powf(2.0f, -10.0f * v);
			break;
		case ease_inSin:
			r = 1.0f - scalar::cos(v * (MATH_PI/2.0f));
			break;
		case ease_outSin:
			r = scalar::sin(v * (MATH_PI/2.0f));
			break;
		case ease_inCubic:
			r = v * v * v;
			break;
		case ease_outCubic:			
			vi = v - 1.0f;
			r = vi * vi * vi + 1.0f;
			break;
		case ease_inOutBack:		
			{
				const float s15 = s * 1.525f;

				v *= 2;
				if (v < 1)
				{
					r = v * v * ((s15 + 1) * v - s15);
				}
				else
				{				
					v-=2;
					r = v * v * ((s15 + 1) * v + s15) + 2;
				}
				r /= 2;
			}
			break;
		case ease_inBack:
			r = v * v * ((s + 1) * v - s);
			break;
		case ease_outBack:
			v -= 1;
			r = v * v *((s + 1)*v + s) + 1;
			break;
		default:
			OX_ASSERT(!"unsupported ease");
			break;
		}

		return r;
	}

	
	float Tween::_calcEase(float v)
	{
		if (_twoSides)
		{
			if (v > 0.5f)
				v = 1.0f - v;
			v *= 2.0f;
		}

		v = calcEase(_ease, v);
		return v;
	}

	void Tween::complete(timeMS deltaTime)
	{
		if (_loops == -1)
			return;

		//if already done
		if (_status >= status_done)
			return;

		OX_ASSERT(_client);

		//OX_ASSERT(!"not implemented");

		//not started yet because if delay
		if (_status == status_delayed)
		{
			_start(*_client);
			_status = status_started;
		}


		OX_ASSERT(_status == status_started);
		//while (_status != status_remove)
		{
			UpdateState us;
			us.dt = deltaTime;

			update(*_client, us);
		}

		OX_ASSERT(_status == status_done);
		
		//_client->removeTween(this);		
	}

	void Tween::start(Actor &actor)
	{
		_client = &actor; 
		_status = status_delayed;
		if (_delay == 0)
		{
			_status = status_started;
			_start(actor);
		}
	}

	void Tween::update(Actor &actor, const UpdateState &us)
	{
		_elapsed += us.dt;
		switch(_status)
		{
		case status_delayed:
			{
				if (_elapsed >= _delay)
				{
					_status = status_started;
					_start(*_client);					
				}
			}
			break;
		case status_started:
			{
				if (_duration)
				{				
					timeMS localElapsed = _elapsed - _delay;

					int loopsDone = localElapsed / _duration;

					_percent = _calcEase(((float)(localElapsed - loopsDone * _duration)) / _duration);

					if (_loops > 0 && int(loopsDone) >= _loops)
					{
						if (_twoSides)
							_percent = 0;
						else
							_percent = 1;

						_status = status_done;
					}
				}
				_update(*_client, us);
			}
			break;
		case status_done:
			{
				done(*_client, us);
			}
			break;
		}
	}

	void Tween::done(Actor &actor, const UpdateState &us)
	{		
		_done(actor, us);
		
		if (_detach)
		{
			actor.detach();
		}

		TweenEvent ev(this, &us);
		ev.target = ev.currentTarget = &actor;
		ev.tween = this;

		if (_cbDone)		
			_cbDone(&ev);

		dispatchEvent(&ev);

		_status = status_remove;
	}

	
	spTween TweenQueue::add(spTween t)
	{
		OX_ASSERT(t);
		if (!t)
			return 0;

		_tweens.append(t);		
		return t;
	}

	void TweenQueue::complete(timeMS deltaTime)
	{
		OX_ASSERT("Tween::complete is not supported for TweenQueue");
	}

	void TweenQueue::_start(Actor &actor)
	{
		_current = _tweens._first;
		if (!_current)
			return;

		_current->start(actor);
	}

	void TweenQueue::_update(Actor &actor, const UpdateState &us)
	{
		_elapsed += us.dt;

		if (_current)
		{
			spTween next = _current->getNextSibling();
			_current->update(actor, us);
			if (_current->isDone())
			{
				_current = next;
				if (_current)				
					_current->start(actor);				
			}
		}

		if (!_current)
			_status = status_done;
	}

	Actor* TweenEvent::getActor() const
	{
		return safeCast<Actor*>(currentTarget.get());
	}
}