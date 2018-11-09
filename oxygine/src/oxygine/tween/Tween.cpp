#include "Tween.h"
#include "../actor/Actor.h"
#include "../actor/ColorRectSprite.h"
#include <limits.h>

namespace oxygine
{
    Tween::Tween() : _duration(0),
        _loops(1),
		_loopsDone(0),
        _percent(0),
        _status(status_not_started),
        _disabledStatusDone(false),
        _elapsed(0), _twoSides(false), _ease(ease_linear), _detach(false), _delay(0), _client(0), _globalEase(ease_linear)
    {

    }

    Tween::~Tween()
    {
    }

    void Tween::reset()
    {
        _elapsed = 0;
        _status = status_not_started;
		_loopsDone = 0;
    }

    void Tween::init(timeMS duration, int loops, bool twoSides, timeMS delay, EASE ease)
    {
        _duration = duration;
        _durationF = duration / 1000.0f;
        _delay  = delay;
        _delayF = delay / 1000.0f;

        _ease = ease;
        _loops = loops;
        _twoSides = twoSides;

        if (_duration <= 0)
        {
            OX_ASSERT(!"Tweener duration should be more than ZERO");
            _duration = 1;
        }
    }

	void Tween::init2(const TweenOptions& opt)
	{
		_duration = opt._duration;
        _durationF = opt._duration / 1000.0f;
        _delay = opt._delay;
        _delayF = opt._delay / 1000.0f;

		_ease = opt._ease;
		_loops = opt._loops;
		_twoSides = opt._twoSides;		
		_detach = opt._detach;
		_globalEase = opt._globalEase;
		_cbDone = opt._callback;

		if (_duration <= 0)
		{
			OX_ASSERT(!"Tweener duration should be more than ZERO");
			_duration = 1;
		}
	}

	void Tween::setDoneCallback(const EventCallback& cb)
    {
        _cbDone = cb;
    }

	void Tween::addDoneCallback(const EventCallback& cb)
    {
        addEventListener(TweenEvent::DONE, cb);
    }



    void Tween::setDelay(timeMS delay)
    {
        _delay = delay;
        _delayF = delay / 1000.0f;
    }

    void Tween::setDuration(timeMS duration)
    {
        _duration = duration;
        _durationF = duration / 1000.0f;
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

	void Tween::remove()
	{
		getClient()->removeTween(this);
	}

    void Tween::complete(timeMS deltaTime)
    {
        if (_loops == -1)
            return;

//if already done
        if (_status >= status_done)
            return;

        //OX_ASSERT(_client);

		if (!_client)
			return;

//OX_ASSERT(!"not implemented");

//not started yet because has delay
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
            us.dtf = deltaTime / 1000.0f;
            us.time = deltaTime;
            us.timef = deltaTime / 1000.0f;

            update(*_client, us);
        }

        OX_ASSERT(_status == status_done);

//_client->removeTween(this);
    }

    void Tween::start(Actor& actor)
    {
        _client = &actor;
        _status = status_delayed;
        if (_delay == 0)
        {
            _status = status_started;
            _start(actor);
        }
    }

    void Tween::update(Actor& actor, const UpdateState& us)
    {
        _elapsed += us.dtf;
        switch (_status)
        {
            case status_delayed:
            {
                if (_elapsed >= _delayF)
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
                    float localElapsed = _elapsed - _delayF;
					
					if (_globalEase != ease_linear)
					{
						float p = localElapsed / (float(_durationF * _loops));
						float nv = calcEase(_globalEase, std::min(p, 1.0f)) * _durationF * _loops;
						localElapsed = nv;
					}

					int loopsDone = (int)(localElapsed / _durationF);
					_percent = _calcEase(((float)(localElapsed - loopsDone * _durationF)) / _durationF);

					while(_loopsDone < loopsDone)
					{
						_loopDone(actor, us);
						_loopsDone++;
					}

                    if (_loops > 0 && int(loopsDone) >= _loops)
                    {
                        if (_twoSides)
                            _percent = 0;
                        else
                            _percent = 1;

                        if (!_disabledStatusDone)
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
            default:
                break;
        }
    }

    void Tween::done(Actor& actor, const UpdateState& us)
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

    Actor* TweenEvent::getActor() const
    {
        return safeCast<Actor*>(currentTarget.get());
    }


    static float outBounce(float t)
    {
        if (t < 0.363636363636f)
        {
            return 7.5625f * t * t;
        }
        if (t < 0.727272727273f)
        {
            t = t - 0.545454545455f;
            return 7.5625f * t * t + 0.75f;
        }
        if (t < 0.909090909091f)
        {
            t = t - 0.818181818182f;
            return 7.5625f * t * t + 0.9375f;
        }
        t = t - 0.954545454545f;
        return 7.5625f * t * t + 0.984375f;
    }


	Tween::easeHandler _customEaseHandler = 0;

    float Tween::calcEase(EASE ease, float t)
    {
        const float s = 1.70158f;

        switch (ease)
        {
            case ease_linear:
                return t;

			#define DEF_EASY_FROM_IN(EasyPost, formula) \
			case ease_in ## EasyPost: \
				return (formula); \
			case ease_out ## EasyPost: \
				return 1 - calcEase(ease_in ## EasyPost, 1 - t); \
			case ease_inOut ## EasyPost: \
				return t <= 0.5f ? calcEase(ease_in ## EasyPost, t * 2) / 2 : 1 - calcEase(ease_in ## EasyPost, 2 - t * 2) / 2; \
			case ease_outIn ## EasyPost: \
				return t <= 0.5f ? calcEase(ease_in ## EasyPost, t * 2) / 2 : 1 - calcEase(ease_in ## EasyPost, 2 - t * 2) / 2; \

				DEF_EASY_FROM_IN(Quad, t * t);
				DEF_EASY_FROM_IN(Cubic, t * t * t);
				DEF_EASY_FROM_IN(Quart, powf(t, 4));
				DEF_EASY_FROM_IN(Quint, powf(t, 5));
				DEF_EASY_FROM_IN(Sin, 1.0f - scalar::cos(t * (MATH_PI / 2.0f)));
				DEF_EASY_FROM_IN(Expo, powf(2, 10 * (t - 1)));
				DEF_EASY_FROM_IN(Circ, -1.0f * (sqrt(1 - t * t) - 1));
				DEF_EASY_FROM_IN(Back, t * t * ((s + 1) * t - s));
				DEF_EASY_FROM_IN(Bounce, 1 - outBounce(1 - t));

			default:
				t = _customEaseHandler(ease, t);
				//OX_ASSERT(!"unsupported ease");
				break;
		}

		#undef DEF_EASY_FROM_IN

		return t;
	}

	void  Tween::setCustomEaseHandler(easeHandler h)
	{
		_customEaseHandler = h;
	}


	std::string ease2String(Tween::EASE ease)
	{
		#define STRINGIFY(x) #x
		#define S(x) STRINGIFY(x)

		switch (ease)
		{
		case Tween::ease_linear:
			return "linear";

		#define DEF_EASY_FROM_IN(EasyPost) \
		case Tween::ease_in ## EasyPost: \
			return  "in" S(EasyPost);\
		case Tween::ease_out ## EasyPost: \
			return "out" S(EasyPost); \
		case Tween::ease_inOut ## EasyPost: \
			return "inOut" S(EasyPost); \
		case Tween::ease_outIn ## EasyPost: \
			return "outIn" S(EasyPost);

			DEF_EASY_FROM_IN(Quad);
			DEF_EASY_FROM_IN(Cubic);
			DEF_EASY_FROM_IN(Quart);
			DEF_EASY_FROM_IN(Quint);
			DEF_EASY_FROM_IN(Sin);
			DEF_EASY_FROM_IN(Expo);
			DEF_EASY_FROM_IN(Circ);
			DEF_EASY_FROM_IN(Back);
			DEF_EASY_FROM_IN(Bounce);
        default:
            break;
		}

		return "unknown";
	}
}