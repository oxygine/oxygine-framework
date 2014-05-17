#include "Draggable.h"
#include "math/AffineTransform.h"
#include "Actor.h"
#include "RootActor.h"

namespace oxygine
{
	Vector2 convertPosUp(Actor *src, Actor *dest, const Vector2 &pos, bool direction)		
	{
		Vector2 locPos = pos;
		AffineTransform t;
		t.identity();
		//t = src->getTransform();
		while (src != dest && src)
		{
			t = src->getTransform() * t;
			src = src->getParent();
		}


		if (direction)
		{
			t.x = 0;
			t.y = 0;
		}

		locPos = t.transform(locPos);
		return locPos;
	}

	Vector2 convertPosDown(Actor *src, Actor *dest, const Vector2 &pos, bool direction)		
	{
		Vector2 locPos = pos;
		AffineTransform t;
		t.identity();
		t = src->getTransform();
		while (src != dest && src)
		{
			t =  t * src->getTransform();
			src = src->getParent();
		}


		if (direction)
		{
			t.x = 0;
			t.y = 0;
		}

		locPos = t.transform(locPos);
		return locPos;
	}

	Draggable::Draggable():_bounds(0,0,-1,-1),
		_dragClient(0), 
		_actor(0),
		_clientPos(0,0),
		_dragPos(0,0), 
		_clientIsParent(false), 
		_dragEnabled(true),
		_pressed(false), 
		_singleDrag(false)
	{

	}

	Draggable::~Draggable()
	{
		destroy();
	}

	void Draggable::destroy()
	{
		if (_actor && !_singleDrag)
		{
			if (getRoot())
				getRoot()->removeEventListeners(this);
			_actor->removeEventListeners(this);
			_actor = 0;
		}

        _pressed = false;
	}

	void Draggable::init(Actor *actor)
	{
        destroy();

		_singleDrag = false;
		_actor = actor;
		_dragClient = actor;

		actor->addEventListener(TouchEvent::TOUCH_DOWN, CLOSURE(this, &Draggable::onEvent));
		actor->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Draggable::onEvent));
	}

	void Draggable::start(const PointerState *pointer, Actor *actor, const Vector2 &localPosition)
	{
		_singleDrag = true;
		_actor = actor;
		_dragClient = actor;

		getRoot()->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Draggable::onEvent));
		
		Vector2 src = pointer->getPosition().cast<Vector2>();
		Vector2 pos = convert_root2local(actor->getParent(), src);
		actor->setPosition(pos - localPosition);

		startDrag(localPosition);
	}

	void Draggable::startDrag(const Vector2 &localCenter)
	{
		_pressed = true;
		_dragPos = localCenter;
		_clientPos = _dragClient->getPosition();

		//why I did add it?
		//event->stopPropagation();

		getRoot()->addEventListener(TouchEvent::MOVE, CLOSURE(this, &Draggable::onEvent));
	}

	void Draggable::onMove(const Vector2 &position)
	{
		if (_pressed && _dragEnabled)
		{
			Actor *client = _dragClient;


			Vector2 localPos = convert_root2local(client, position);

			Vector2 dragOffset = localPos - _dragPos;

			Vector2 converted = convertPosUp(client, client->getParent(), dragOffset, true);
			//printVec("dragOffset", dragOffset);
			//printVec("converted", converted);
			Vector2 np;
			bool _clientIsParent = true;
			if (!_clientIsParent)
				np = _clientPos + dragOffset;
			else 
				np = client->getPosition() + converted;

			client->setPosition(np);


			snapClient2Bounds();
		}
	}

	void Draggable::onEvent(Event *event)
	{
		//if (event->currentTarget.get() != _actor)
		//	return;

		TouchEvent *te = safeCast<TouchEvent*>(event);
		switch(te->type)
		{
		case TouchEvent::TOUCH_DOWN:
			{
				startDrag(te->localPosition);
			}
			break;
		case TouchEvent::TOUCH_UP:
			{
				_pressed = false;
				getRoot()->removeEventListeners(this);
			}
			break;

		case TouchEvent::MOVE:
			{
				onMove(te->localPosition);
			}
			break;
		}
	}

	void Draggable::setDragClient(Actor *actor)
	{
		_dragClient = actor;
	}

	void Draggable::setDragBounds(const RectF &r)
	{
		_bounds = r;
	}

	bool isParent(Actor *child, Actor *isItParent)
	{
		while (child)
		{
			if (child == isItParent)
				return true;
			child = child->getParent();
		}

		return false;
	}

	/*
	void Draggable::onPress(TouchEvent *es, bool press)
	{
		if (press)
		{
			_dragPos = es.localPosition;
			_clientPos = getClient()->getPosition();

			_clientIsParent = isParent(es.actor, getClient());
		}			
	}*/

	Actor *Draggable::getClient()
	{
		Actor *actor = _dragClient ? _dragClient : _actor;
		return actor;
	}

	void printVec(const char *str, const Vector2 &v)
	{
		log::messageln("%s: %.2f, %.2f", str, v.x, v.y);
	}


	void Draggable::snapClient2Bounds()
	{
		Actor *client = getClient();

		Vector2 np = client->getPosition();
		if (_bounds.getWidth() != -1 && _bounds.getHeight() != -1)
		{
			np.x = max(np.x, (float)_bounds.getX());
			np.y = max(np.y, (float)_bounds.getY());

			np.x = min(np.x, (float)_bounds.getRight());
			np.y = min(np.y, (float)_bounds.getBottom());
		}
		client->setPosition(np);
	}

}
