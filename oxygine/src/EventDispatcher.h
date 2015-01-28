#pragma once
#include "oxygine_include.h"
#include <list>
#include "core/Object.h"
#include "closure/closure.h"

namespace oxygine
{
	typedef int eventType;
	class Event;

#define makefourcc(a,b,c,d) ( ((unsigned int)a) | (((unsigned int)b)<< 8) | (((unsigned int)c)<<16) | (((unsigned int)d)<<24))

	//eventID('_', '_', '_', '_')
#define eventID(a,b,c,d) makefourcc(a,b,c,d)

	/*sysEventID is used for system Oxygine events, use 'eventID' for custom game events*/
#define sysEventID(b,c,d) makefourcc(0xA,b,c,d)


	typedef Closure<void (Event *ev)> EventCallback;

	DECLARE_SMART(EventDispatcher, spEventDispatcher);
	class EventDispatcher: public _Object
	{
	public:
		EventDispatcher(const EventDispatcher &ed):_Object(ed), _lastID(0), _listeners(0){}
		EventDispatcher();
		~EventDispatcher();

		int addEventListener(eventType, EventCallback);
		void removeEventListener(eventType, EventCallback);
		void removeEventListener(int id);
		bool hasEventListeners(void *CallbackThis);
		void removeEventListeners(void *CallbackThis);
		void removeAllEventListeners();

		virtual void dispatchEvent(Event *event);

		int getListenersCount() const {if (!_listeners) return 0; return (int)_listeners->size();}

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
