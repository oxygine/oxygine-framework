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


    typedef Closure<void (Event* ev)> EventCallback;

    DECLARE_SMART(EventDispatcher, spEventDispatcher);
    class EventDispatcher: public _Object
    {
    public:
        EventDispatcher(const EventDispatcher& ed): _Object(ed), _lastID(0), _listeners(0) {}
        EventDispatcher();
        ~EventDispatcher();

        int addEventListener(eventType, const EventCallback&);

        /**remove event listener by event type with callback, not working with lambda functions, works only for CLOSURE(...)*/
        void removeEventListener(eventType, const EventCallback&);

        /**remove by ID, where is ID returned from addEventListener*/
        void removeEventListener(int id);

        /**check if event listener exists by THIS used in CLOSURE(this, ...), not working with labmda*/
        bool hasEventListeners(void* CallbackThis);

        /**check if event listener exists, not working with lambda, works only with CLOSURE(...)*/
        bool hasEventListeners(eventType, const EventCallback&);

        /**removes all added event listeners by THIS used in CLOSURE(this, ...)*/
        void removeEventListeners(void* CallbackThis);

        /**removes all added event listeners*/
        void removeAllEventListeners();

        virtual void dispatchEvent(Event* event);

        int getListenersCount() const;
        int getLastListenerID() const { return _lastID; }


    protected:

        struct listenerbase
        {
            EventCallback cb;
            int id;
        };

        struct listener : public listenerbase
        {
            eventType type;
        };

        int _lastID;

        typedef std::vector<listener> listeners;
        listeners* _listeners;
    };
}
