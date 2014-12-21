#include "EventDispatcher.h"
#include "Event.h"
namespace oxygine
{
	EventDispatcher::EventDispatcher():_lastID(0), _listeners(0)
	{

	}

	EventDispatcher::~EventDispatcher()
	{
		__doCheck();
		delete _listeners;
	}

	int EventDispatcher::addEventListener(eventType et, EventCallback cb)
	{
		__doCheck();

		if (!_listeners)
			_listeners = new listeners;

		_lastID++;

		/*
#ifdef OX_DEBUG
		for (listeners::iterator i = _listeners->begin(); i != _listeners->end(); ++i)
		{
			const listener& ls = *i;
			if (ls.type == et && cb == ls.cb)
			{
				OX_ASSERT(!"you are already added this event listener");
			}
		}
#endif
		*/

		listener ls;
		ls.type = et;
		ls.cb = cb;
		ls.id = _lastID;
		_listeners->push_back(ls);

		return ls.id;
	}

	void EventDispatcher::removeEventListener(int id)
	{
		__doCheck();

		OX_ASSERT(_listeners);
		if (!_listeners)
			return;

		for (listeners::iterator i = _listeners->begin(); i != _listeners->end(); )
		{
			const listener &ls = *i;
			if (ls.id == id)
			{
				i = _listeners->erase(i);
				break;
			}
			else
				++i;
		}
	}

	void EventDispatcher::removeEventListener(eventType et, EventCallback cb)
	{
		__doCheck();

		//OX_ASSERT(_listeners);
		if (!_listeners)
			return;

		for (listeners::iterator i = _listeners->begin(); i != _listeners->end(); )
		{
			listener ls = *i;
			if (ls.type == et && cb == ls.cb)
			{
				i = _listeners->erase(i);
			}
			else
				++i;
		}
	}

	void EventDispatcher::removeEventListeners(void *CallbackThis)
	{
		__doCheck();
		//OX_ASSERT(_listeners);
		if (!_listeners)
			return;

		for (listeners::iterator i = _listeners->begin(); i != _listeners->end(); )
		{
			listener ls = *i;
			if (ls.cb.p_this == CallbackThis)
			{
				i = _listeners->erase(i);
			}
			else
				++i;
		}
	}

	void EventDispatcher::removeAllEventListeners()
	{
		delete _listeners;
		_listeners = 0;
	}

	
	void EventDispatcher::dispatchEvent(Event *event_)
	{
		__doCheck();
		if (!_listeners)
			return;

		Event *ev = event_;
		ev->currentTarget = this;

		listeners copy = *_listeners;//todo, do something!
		for (listeners::iterator i = copy.begin(); i != copy.end(); ++i)
		{
			listener ls = *i;
			if (ls.type != ev->type)
				continue;
			//todo!

			/*
			bool found = false;
			for (listeners::iterator n = _listeners->begin(); n != _listeners->end(); ++n)
			{
				const listener &ols= *n;
				if (ols.type == ls.type && ols.id == ls.id && ols.cb == ls.cb)
				{
					found = true;
					break;
				}
			}

			if (!found)
				continue;

			*/

			//OX_ASSERT(ls.type < 20);
			//log::messageln("cb: %d %x", ls.type, ls.cb.p_this);
			ls.cb(ev);
			if (ev->stopsImmediatePropagation)
			{
				break;
			}
		}
	}
}