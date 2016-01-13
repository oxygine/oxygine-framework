#pragma once
#include "oxygine_include.h"
#include "core/Object.h"
#include "closure/closure.h"
#include "utils/intrusive_list.h"
#include "UpdateState.h"

#include "EventDispatcher.h"
#include "Event.h"
#include <stdint.h>
#include <limits>
#include "Property.h"

namespace oxygine
{
    class Tween;
    class Actor;
    class UpdateState;

    class TweenEvent : public Event
    {
    public:
        enum
        {
            DONE = sysEventID('T', 'W', 'D')
        };

        TweenEvent(Tween* tween_, const UpdateState* us_) : Event(DONE, false), tween(tween_), us(us_) {}
        Actor* getActor() const;

        Tween* tween;
        const UpdateState* us;
    };

    class UpdateState;
    class TweenOptions;


    DECLARE_SMART(Tween, spTween);

    class Tween : public EventDispatcher, public intrusive_list_item<spTween>
    {
        typedef intrusive_list_item<spTween> intr_list;
    public:
        enum EASE
        {
            ease_unknown,
            ease_linear,

            ease_inQuad,
            ease_outQuad,
            ease_inOutQuad,
            ease_outInQuad,

            ease_inCubic,
            ease_outCubic,
            ease_inOutCubic,
            ease_outInCubic,

            ease_inQuart,
            ease_outQuart,
            ease_inOutQuart,
            ease_outInQuart,

            ease_inQuint,
            ease_outQuint,
            ease_inOutQuint,
            ease_outInQuint,

            ease_inSin,
            ease_outSin,
            ease_inOutSin,
            ease_outInSin,

            ease_inExpo,
            ease_outExpo,
            ease_inOutExpo,
            ease_outInExpo,

            ease_inCirc,
            ease_outCirc,
            ease_inOutCirc,
            ease_outInCirc,

            ease_inBack,
            ease_outBack,
            ease_inOutBack,
            ease_outInBack,

            ease_inBounce,
            ease_outBounce,
            ease_inOutBounce,
            ease_outInBounce,

            ease_count
        };

        Tween();
        ~Tween();

        void init(timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, EASE ease = Tween::ease_linear);//todo twoSide find better name
        void init2(const TweenOptions& opt);
        /**if you reset internal Tween state it could be reused and added to actor again */
        void reset();

        int         getLoops() const { return _loops; }
        timeMS      getDuration() const { return _duration; }
        timeMS      getElapsed() const { return _elapsed; }
        EASE        getEase() const { return _ease; }
        timeMS      getDelay() const { return _delay; }
        Actor*      getClient() const { return _client; }
        float       getPercent() const { return _percent; }
        spObject    getDataObject() const { return _data; }
        spTween&    getNextSibling() { return intr_list::getNextSibling(); }
        spTween&    getPrevSibling() { return intr_list::getPrevSibling(); }

        bool        isStarted() const { return _status != status_not_started; }
        bool        isDone() const { return _status == status_remove; }

        /**set custom user data object to Tween. Could be used for store some useful data*/
        void setDataObject(spObject data) { _data = data; }
        /**add callback would be called when tween done.  Could be added more than one.
        setDoneCallback is faster because it doesn't allocate memory for list internally
        */
        void addDoneCallback(const EventCallback& cb);
        /**set Easing function*/
        void setEase(EASE ease) { _ease = ease; }
        /**set Delay before starting tween*/
        void setDelay(timeMS delay) { _delay = delay; }
        /** loops = -1 means infinity repeat cycles*/
        void setLoops(int loops) { _loops = loops; }
        /*set Duration of tween**/
        void setDuration(timeMS duration) { _duration = duration; }
        void setClient(Actor* client) { _client = client; }
        void setTwoSides(bool ts) { _twoSides = ts; }

        /**delete actor from parent node when tween done*/
        void setDetachActor(bool detach) { _detach = detach; }

        /**immediately completes tween, calls doneCallback and mark tween as completed and removes self from Actor. If tween has infinity loops (=-1) then do nothing*/
        virtual void complete(timeMS deltaTime = std::numeric_limits<int>::max() / 2);


        void start(Actor& actor);
        void update(Actor& actor, const UpdateState& us);

        static float calcEase(EASE ease, float v);

        /**set callback when tween done. Doesn't allocate memory. faster than addDoneCallback*/
        void setDoneCallback(const EventCallback& cb);

    protected:
        void done(Actor&, const UpdateState& us);

        virtual void _start(Actor& actor) {}
        virtual void _update(Actor& actor, const UpdateState& us) {}
        virtual void _done(Actor& actor, const UpdateState& us) {}
        virtual void _loopDone(Actor& actor, const UpdateState& us) {}
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
        int _loopsDone;
        EASE _ease;
        bool _twoSides;

        float _percent;
        bool _detach;

        EventCallback _cbDone;
        Actor* _client;

        spObject _data;
    };

    template<class GS>
    class TweenT : public Tween
    {
    public:
        typedef typename GS::type type;

        TweenT(const GS& gs) : _gs(gs) {}

        void _update(Actor& actor, const UpdateState& us)
        {
            type& t = *safeCast<type*>(&actor);
            _gs.update(t, _percent, us);//todo fix cast
        }

        void _start(Actor& actor)
        {
            type& t = *safeCast<type*>(&actor);
            _gs.init(t);
            UpdateState us;
            us.iteration = -1;
            _gs.update(t, _calcEase(0.0f), us);
        }

        void _done(Actor& actor, const UpdateState& us)
        {
            type& t = *safeCast<type*>(&actor);
            _gs.done(t);
        }

        GS& getGS() { return _gs; }

    private:
        GS _gs;
    };


    template <typename GS>
    TweenT<GS>* createTween(const GS& gs, timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, Tween::EASE ease = Tween::ease_linear)
    {
        TweenT<GS>* p = new TweenT<GS>(gs);
        p->init(duration, loops, twoSides, delay, ease);
        return p;
    }

    template <typename GS>
    TweenT<GS>* createTween2(const GS& gs, const TweenOptions& opt)
    {
        TweenT<GS>* p = new TweenT<GS>(gs);
        p->init2(opt);
        return p;
    }

    std::string ease2String(Tween::EASE ease);
}
