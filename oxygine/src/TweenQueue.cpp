#include "TweenQueue.h"

namespace oxygine
{
	spTweenQueue TweenQueue::create(spTween t1, spTween t2)
	{
		spTweenQueue t = new TweenQueue;
		t->add(t1);
		t->add(t2);
		return t;
	}

	spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3)
	{
		spTweenQueue t = new TweenQueue;
		t->add(t1);
		t->add(t2);
		t->add(t3);
		return t;
	}

	spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3, spTween t4)
	{
		spTweenQueue t = new TweenQueue;
		t->add(t1);
		t->add(t2);
		t->add(t3);
		t->add(t4);
		return t;
	}

	spTweenQueue TweenQueue::create(spTween t1, spTween t2, spTween t3, spTween t4, spTween t5)
	{
		spTweenQueue t = new TweenQueue;
		t->add(t1);
		t->add(t2);
		t->add(t3);
		t->add(t4);
		t->add(t5);
		return t;
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
}