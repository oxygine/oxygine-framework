#pragma once
#include "Tween.h"

namespace oxygine
{
    DECLARE_SMART(TweenQueue, spTweenQueue);
    class TweenQueue : public Tween
    {
    public:

        enum { EVENT_LOOP_BEGIN = sysEventID('T', 'L', 'B'), };
        enum { EVENT_LOOP_END = sysEventID('T', 'L', 'E'), };

        TweenQueue();

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1);

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1, spTween t2);

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1, spTween t2, spTween t3);

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1, spTween t2, spTween t3, spTween t4);

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5);

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5, spTween t6);

        /**short syntax for easy tween queue creation*/
        static spTweenQueue create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5, spTween t6, spTween t7);

        /*Add new new tween to Queue, returns added tween*/
        spTween add(spTween t);

        template<class GS>
        spTween add(const GS& gs, timeMS duration, int loops = 1, bool twoSides = false, timeMS delay = 0, Tween::EASE ease = Tween::ease_linear);

        void complete(timeMS deltaTime);

    private:
        void _start(Actor& actor);
        void _update(Actor& actor, const UpdateState& us);

        typedef intrusive_list<spTween> tweens;
        tweens _tweens;
        spTween _current;
        int _loopsDone;
    };


    template<class GS>
    spTween TweenQueue::add(const GS& gs, timeMS duration, int loops, bool twoSides, timeMS delay, Tween::EASE ease)
    {
        return add(createTween(gs, duration, loops, twoSides, delay, ease));
    }
}