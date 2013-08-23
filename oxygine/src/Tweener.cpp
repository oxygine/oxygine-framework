#include "Tweener.h"
#include "Actor.h"
#include "ColorRectSprite.h"
#include <limits.h>
namespace oxygine
{
	Tween::Tween():_duration(0), 
		_loops(0), 
		_percent(0), 
		_value(0), 
		_done(false), _started(false), _relative(false),
		_elapsed(0), _twoSides(false), _ease(ease_linear), _remove(false), _delay(0), _client(0)
	{

	}

	Tween::~Tween()
	{
	}

	void Tween::reset()
	{
		_elapsed = 0;
		_done = false;
		_started = false;
	}

	void Tween::init(timeMS duration, int loops, bool twoSides, timeMS delay, EASE ease)
	{
		_done = false;
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

	float Tween::calcEase(EASE ease, float v)
	{
		float vi;
		float r = 0;
		float s = 1.70158f;// * 1.525f + 1;

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
			v *= 2;
			if (v < 1)
			{
				r = v * v * (( (s*=1.525f) + 1) * v - s);
			}
			else
			{				
				v-=2;
				r = v * v * (((s *= 1.525f) + 1) * v + s) + 2;
			}
			r /= 2;
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

		if (!_started)
			return;//todo fix, tween with delay wasn't started yet

		OX_ASSERT(_client);

		UpdateState us;

		us.dt = deltaTime;
		bool done = update(*_client, us);
		if (!done)
		{
			callDone(*_client, &us);
		}
		_client->removeTween(this);
	}

	void Tween::callDone(Actor &actor, const UpdateState *us)
	{
		OX_ASSERT(_client);

		if (_remove)
		{
			actor.detach();
		}
		if (_cbDone)
		{
			TweenEvent ev(this, us);
			ev.target = ev.currentTarget = &actor;
			ev.tween = this;

			_cbDone(&ev);					
		}
	}

	bool Tween::start(Actor &actor)
	{
		_client = &actor; 
		_started = true; 

		return false;
	}

	bool Tween::itDone(Actor &t, const UpdateState &us)
	{
		OX_ASSERT(_client);
		//if (us.iteration == 0)//wait next cycle update/render
		{
			callDone(*_client, &us);
			_client = 0;
			return true;
		}			

		return false; 
	}

	bool Tween::update(Actor &actor, const UpdateState &us)
	{
		if (_done)		
		{
			return itDone(actor, us);
		}

		_elapsed += us.dt;

		if (!_started)
		{
			if (_elapsed >= _delay)
			{
				bool done = start(actor);
				return done;
			}
			else
			{
				return false;
			}
		}

		OX_ASSERT(_client);

		timeMS localElapsed = _elapsed - _delay;

		int loopsDone = localElapsed / _duration;
		_percent = ((float)(localElapsed - loopsDone * _duration)) / _duration;

				
		_percent = _calcEase(_percent);

		bool done = false;

		if (_loops > 0 && int(loopsDone) >= _loops)
		{
			if (_twoSides)
				_percent = 0;
			else
				_percent = 1;

			done = true;
		}
		_value = _percent;		
		_done = done;

		//printf("loopsDone, percent: %d, %.4f\n", loopsDone, _percent);

		return false;
	}

	
	TweenQueue *TweenQueue::add(spTween t)
	{
		OX_ASSERT(t);
		if (!t)
			return 0;

		_tweens.append(t);		
		return this;
	}

	bool TweenQueue::start(Actor &actor)
	{
		OX_ASSERT(_relative == false);
		Tween::start(actor);
		_current = _tweens._first;
		if(_current->getDelay() == 0)
			return _current->start(actor);
		return false;
	}

	bool TweenQueue::update(Actor &actor, const UpdateState &us)
	{
		_elapsed += us.dt;
		if (_elapsed < _delay)
			return false;
		if (!_started)
		{
			start(actor);
		}

		while (_current)
		{
			spTween next = _current->getNextSibling();
			bool done = _current->update(actor, us);
			if (done)
			{
				_current = next;
			}
			else
			{
				break;
			}
		}

		bool done = _current == 0;
		_done = done;
		if (done)
		{
			callDone(actor, &us);
		}
		
		return done;
	}

	Actor* TweenEvent::getActor() const
	{
		return safeCast<Actor*>(currentTarget.get());
	}
}