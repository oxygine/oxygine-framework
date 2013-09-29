#pragma once
#include "oxygine_include.h"
#include <list>
#include "core/Object.h"
#include "closure/closure.h"

namespace oxygine
{
	typedef int eventType;
	DECLARE_SMART(EventDispatcher, spEventDispatcher);	



	//DECLARE_SMART(Event, spNewEvent);
	class Event//: public Object
	{
	public:
		enum {COMPLETE = makefourcc('_', 'E', 'C', 'M')};

		enum Phase
		{
			phase_capturing = 1,
			phase_target,
			phase_bubbling
		};

		eventType type;
		Phase phase;
		bool bubbles;
		bool stopsImmediatePropagation;
		bool stopsPropagation;

		spEventDispatcher target;
		spEventDispatcher currentTarget;// = object with our listener

		Event(eventType Type, bool Bubbles = false):type(Type), phase(phase_target), bubbles(Bubbles), stopsImmediatePropagation(false), stopsPropagation(false){}

		virtual ~Event(){}

		void stopPropagation(){stopsPropagation = true;}
		void stopImmediatePropagation(){stopsPropagation = stopsImmediatePropagation = true;}

		virtual Event* clone(){return new Event(*this);}
		virtual void dump()
		{

		}
	};

	

	typedef Closure<void (Event *ev)> EventCallback;

	class EventDispatcher: public Object
	{
	public:
		EventDispatcher(const EventDispatcher &ed):Object(ed), _lastID(0), _listeners(0){}
		EventDispatcher();
		~EventDispatcher();

		int addEventListener(eventType, EventCallback);
		void removeEventListener(eventType, EventCallback);
		void removeEventListener(int id);
		void removeEventListeners(void *CallbackThis);
		void removeAllEventListeners();

		virtual void dispatchEvent(Event *event);

		int getListenersCount() const {if (!_listeners) return 0; return _listeners->size();}

	protected:

		struct listener
		{
			EventCallback cb;
			eventType type;
			int id;
		};

		int _lastID;
		
		typedef std::list<listener> listeners;
		listeners *_listeners;
	};
}