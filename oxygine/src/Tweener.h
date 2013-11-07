#pragma once
#include "oxygine_include.h"
#include "core/oxygine.h"
#include "core/Object.h"
#include "closure/closure.h"
#include "utils/intrusive_list.h"
#include "UpdateState.h"

#include "EventDispatcher.h"
#include "Event.h"
#include <stdint.h>
#include <limits>

namespace oxygine
{	
	template<typename value, typename valueRef, typename C, valueRef (C::*GetF) () const, void (C::*SetF)(valueRef)>
	class GetSet
	{
	public:
		typedef C type;


		GetSet(valueRef dest):_dest(dest), _initialized(false){}

		void init(type &t, bool relative)
		{
			_initialized = true;
			_src = get(t);
			if (relative)
				_dest = _src + _dest;
		}

		void update(type &t, float p, const UpdateState &us)
		{
			OX_ASSERT(_initialized);			
			value v = interpolate(_src, _dest, p);
			set(t, v);
		}

	private:
		value _dest;
		value _src;
		bool _initialized;

		static valueRef get(C &c) 
		{
			return (c.*GetF)();
		}

		static void set(C &c, valueRef v) 
		{
			return (c.*SetF)(v);
		}		
	};

	template<typename value0, typename value, typename valueRef, typename C, valueRef (C::*GetF) () const, void (C::*SetF)(valueRef)>
	class GetSet2Args: public GetSet<value, valueRef, C, GetF, SetF>
	{
		typedef GetSet<value, valueRef, C, GetF, SetF> GS;
	public:
		GetSet2Args(value0 v1, value0 v2):GS(value(v1, v2)){}
		GetSet2Args(valueRef v):GS(v){}
	};

	template<typename value0, typename value, typename valueRef, typename C, valueRef (C::*GetF) () const, void (C::*SetF)(valueRef)>
	class GetSet2Args1Arg: public GetSet<value, valueRef, C, GetF, SetF>
	{
		typedef GetSet<value, valueRef, C, GetF, SetF> GS;
	public:
		GetSet2Args1Arg(value0 v):GS(value(v, v)){}
		GetSet2Args1Arg(value0 v1, value0 v2):GS(value(v1, v2)){}
		GetSet2Args1Arg(valueRef v):GS(v){}
	};

	class Tween;
	class Actor;
	class UpdateState;

	class TweenEvent: public Event
	{
	public:
		enum 
		{
			DONE = _et_Complete
		};

		TweenEvent(Tween *tween_, const UpdateState *us_):Event(DONE, false), tween(tween_), us(us_){}
		Actor* getActor() const;

		Tween *tween;
		const UpdateState *us;
	};

	class UpdateState;
	

	DECLARE_SMART(Tween, spTween);

	class Tween: public EventDispatcher, public intrusive_list_item<spTween>
	{
		typedef intrusive_list_item<spTween> intr_list;
	public:
		enum EASE
		{
			ease_unknown,
			ease_linear,
			ease_inExpo,
			ease_outExpo,
			ease_inSin,
			ease_outSin,
			ease_inCubic,
			ease_outCubic,
			
			ease_inOutBack,
			ease_inBack,
			ease_outBack,

			ease_count
		};
		
		Tween();
		~Tween();

		void init(timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, EASE ease = Tween::ease_linear);//todo twoSide find better name
		void reset();

		int			getLoops() const {return _loops;}
		timeMS		getDuration() const {return _duration;}
		EASE		getEase() const {return _ease;}
		timeMS		getDelay() const {return _delay;}
		Actor*		getClient() const {return _client;}
		float		getPercent() const {return _percent;}
		spObject	getDataObject() const {return _data;}
		spTween&	getNextSibling() {return intr_list::getNextSibling();}
		spTween&	getPrevSibling() {return intr_list::getPrevSibling();}

		bool		isStarted() const {return _status != status_not_started;}
		bool		isDone() const {return _status == status_remove;}

		void setDataObject(spObject data) {_data = data;}
		void setDoneCallback(EventCallback cb);//deprecated. use tween->addEventListener(TweenEvent::DONE, ...)
		void setEase(EASE ease){_ease = ease;}
		void setDelay(timeMS delay){_delay = delay;}
		/** loops = -1 means infinity repeat cycles*/
		void setLoops(int loops){_loops = loops;}
		void setClient(Actor *client){_client = client;}

		/**delete actor from parent node when tween done*/
		void setDetachActor(bool detach){_detach = detach;}

		/**immediately completes tween, calls doneCallback and mark tween as completed and removes self from Actor. If tween has infinity loops (=-1) then do nothing*/
		virtual void complete(timeMS deltaTime = std::numeric_limits<int>::max()/2);

		
		void start(Actor &actor);
		void update(Actor &actor, const UpdateState &us);	

		static float calcEase(EASE ease, float v);

	protected:
		void done(Actor &, const UpdateState &us);

		virtual void _start(Actor &actor){}
		virtual void _update(Actor &actor, const UpdateState &us){}
		virtual void _done(Actor &actor, const UpdateState &us){}
		virtual float _calcEase(float v);

		enum status
		{
			status_not_started,
			status_delayed,
			status_started,
			status_done,
			status_remove,
		};
		status _status;		
		timeMS _elapsed;

		timeMS _duration;
		timeMS _delay;
		int _loops;
		EASE _ease;		
		bool _twoSides;

		float _percent;
		bool _detach;		
				
		EventCallback _cbDone;
		Actor *_client;	

		spObject _data;		
	};	

	template<class GS>
	class TweenT: public Tween
	{
	public:
		typedef typename GS::type type;

		TweenT(const GS &gs):_gs(gs){}

		void _update(Actor &actor, const UpdateState &us)
		{
			type &t = *safeCast<type*>(&actor); 
			_gs.update(t, _percent, us);//todo fix cast
		}

		void _start(Actor &actor)
		{
			type &t = *safeCast<type*>(&actor);
			_gs.init(t, false);
			UpdateState us;
			us.iteration = -1;
			_gs.update(t, _calcEase(0.0f), us);
		}

		GS &getGS(){return _gs;}

	private:
		GS _gs;
	};

	DECLARE_SMART(TweenQueue, spTweenQueue);
	class TweenQueue: public Tween
	{
	public:
		spTween add(spTween t);

		template<class GS>
		spTween add(const GS &gs, timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, Tween::EASE ease = Tween::ease_linear)
		{return add(createTween(gs, duration, loops, twoSides, delay, ease));}
		
	private:
		void complete(timeMS deltaTime);
		void _start(Actor &actor);
		void _update(Actor &actor, const UpdateState &us);

		typedef intrusive_list<spTween> tweens;
		tweens _tweens;
		spTween _current;
	};

	template <typename GS>
	TweenT<GS> *createTween(const GS &gs, timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, Tween::EASE ease = Tween::ease_linear)
	{
		TweenT<GS> *p = new TweenT<GS>(gs);
		p->init(duration, loops, twoSides, delay, ease);
		return p;
	}
}