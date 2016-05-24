#include "TweenQueue.h"

namespace oxygine
{
    spTweenQueue TweenQueue::create(spTween t1)
    {
        spTweenQueue t = new TweenQueue;
        t->add(t1);
        return t;
    }

    spTweenQueue TweenQueue::create(spTween t1, spTween t2)
    {
        spTweenQueue t = create(t1);
        t->add(t2);
        return t;
    }

    spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3)
    {
        spTweenQueue t = create(t1, t2);
        t->add(t3);
        return t;
    }

    spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3, spTween t4)
    {
        spTweenQueue t = create(t1, t2, t3);
        t->add(t4);
        return t;
    }

    spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5)
    {
        spTweenQueue t = create(t1, t2, t3, t4);
        t->add(t5);
        return t;
    }

    spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5, spTween t6)
    {
        spTweenQueue t = create(t1, t2, t3, t4, t5);
        t->add(t6);
        return t;
    }

    spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5, spTween t6, spTween t7)
    {
        spTweenQueue t = create(t1, t2, t3, t4, t5, t6);
        t->add(t7);
        return t;
    }

    TweenQueue::TweenQueue(): _loopsDone(0) {}

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
        UpdateState us;
        us.dt = deltaTime;
        while (_status != status_done)
        {
            update(*_client, us);
        }
    }

    void TweenQueue::_start(Actor& actor)
    {
        _current = _tweens._first;
        if (!_current)
            return;

        Event ev(EVENT_LOOP_BEGIN);
        dispatchEvent(&ev);

        _current->start(actor);
    }

    void TweenQueue::_update(Actor& actor, const UpdateState& us)
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
        {
            Event ev(EVENT_LOOP_END);
            dispatchEvent(&ev);

            _loopsDone++;

            if (_loopsDone >= _loops && _loops > 0)
            {
                _status = status_done;
            }
            else
            {
                spTween next = _tweens._first;
                while (next)
                {
                    next->reset();
                    next = next->getNextSibling();
                }

                _start(actor);
            }
        }
    }
}