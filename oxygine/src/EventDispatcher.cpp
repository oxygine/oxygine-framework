#include "EventDispatcher.h"
#include "Event.h"
#ifdef __MINGW32__
#include <malloc.h>
#endif

namespace oxygine
{
    EventDispatcher::EventDispatcher(): _lastID(0), _listeners(0)
    {

    }

    EventDispatcher::~EventDispatcher()
    {
        __doCheck();
        delete _listeners;
    }

    int EventDispatcher::addEventListener(eventType et, const EventCallback& cb)
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

        for (size_t size = _listeners->size(), i = 0; i != size; ++i)
        {
            const listener& ls = _listeners->at(i);
            if (ls.id == id)
            {
                _listeners->erase(_listeners->begin() + i);
                break;
            }
        }
    }

    void EventDispatcher::removeEventListener(eventType et, const EventCallback& cb)
    {
        __doCheck();

        //OX_ASSERT(_listeners);
        if (!_listeners)
            return;

        for (size_t size = _listeners->size(), i = 0; i != size; ++i)
        {
            const listener& ls = _listeners->at(i);
            if (ls.type == et && cb == ls.cb)
            {
                _listeners->erase(_listeners->begin() + i);
                break;
                //OX_ASSERT(hasEventListeners(et, cb) == false);
                //--i;
            }
        }
    }

    bool EventDispatcher::hasEventListeners(void* CallbackThis)
    {
        __doCheck();
        if (!_listeners)
            return false;

        for (size_t size = _listeners->size(), i = 0; i != size; ++i)
        {
            const listener& ls = _listeners->at(i);
            if (ls.cb.p_this == CallbackThis)
                return true;
        }
        return false;
    }

    bool EventDispatcher::hasEventListeners(eventType et, const EventCallback& cb)
    {
        __doCheck();
        if (!_listeners)
            return false;

        for (size_t size = _listeners->size(), i = 0; i != size; ++i)
        {
            const listener& ls = _listeners->at(i);
            if (ls.type == et && cb == ls.cb)
                return true;
        }
        return false;
    }

    void EventDispatcher::removeEventListeners(void* CallbackThis)
    {
        __doCheck();
        if (!_listeners)
            return;

        for (size_t i = 0; i < _listeners->size(); ++i)
        {
            const listener& ls = _listeners->at(i);
            if (ls.cb.p_this == CallbackThis)
            {
                _listeners->erase(_listeners->begin() + i);
                //OX_ASSERT(hasEventListeners(CallbackThis) == false);
                --i;
            }
        }
    }

    void EventDispatcher::removeAllEventListeners()
    {
        delete _listeners;
        _listeners = 0;
    }


    void EventDispatcher::dispatchEvent(Event* event)
    {
        if (!event->target)
            event->target = this;

        __doCheck();
        if (!_listeners)
            return;


        size_t size = _listeners->size();
        listenerbase* copy = (listenerbase*)alloca(sizeof(listenerbase) * size);

        size_t num = 0;
        for (size_t i = 0; i != size; ++i)
        {
            listener& ls = _listeners->at(i);
            if (ls.type != event->type)
                continue;

            new(copy + num) listenerbase(ls);
            ++num;
        }

        for (size_t i = 0; i != num; ++i)
        {
            listenerbase& ls = copy[i];
            event->currentTarget = this;
            ls.cb(event);
            if (event->stopsImmediatePropagation)
                break;
        }

        for (size_t i = 0; i != num; ++i)
        {
            listenerbase& ls = copy[i];
            ls.~listenerbase();
        }
    }
}