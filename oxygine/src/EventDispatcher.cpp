#include "EventDispatcher.h"
#include "Event.h"
#ifdef __MINGW32__
#include <malloc.h>
#endif

#define USE_ALLOCA

namespace oxygine
{
    template<class T>
    EventDispatcherT<T>::EventDispatcherT(): _lastID(0), _listeners(0)
    {

    }

    template<class T>
    EventDispatcherT<T>::~EventDispatcherT()
    {
        __doCheck();
        delete _listeners;
    }

    template<class T>
    int EventDispatcherT<T>::addEventListener(eventType et, const EventCallback& cb)
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

    template<class T>
    void EventDispatcherT<T>::removeEventListener(int id)
    {
        __doCheck();

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

    template<class T>
    void EventDispatcherT<T>::removeEventListener(eventType et, const EventCallback& cb)
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

    template<class T>
    bool EventDispatcherT<T>::hasEventListeners(void* CallbackThis)
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

    template<class T>
    bool EventDispatcherT<T>::hasEventListeners(eventType et, const EventCallback& cb)
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

    template<class T>
    void EventDispatcherT<T>::removeEventListeners(void* CallbackThis)
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

    template<class T>
    void EventDispatcherT<T>::removeAllEventListeners()
    {
        delete _listeners;
        _listeners = 0;
    }


    template<class T>
    void EventDispatcherT<T>::dispatchEvent(Event* event)
    {
//        if (!event->target)
  //          event->target = this;

        __doCheck();
        if (!_listeners)
            return;


        size_t size = _listeners->size();
        size_t num = 0;

#ifdef USE_ALLOCA
        listenerbase* copy = (listenerbase*)alloca(sizeof(listenerbase) * size);
#else
        listenerbase* copy = new listenerbase[size];
#endif


        for (size_t i = 0; i != size; ++i)
        {
            listener& ls = _listeners->at(i);
            if (ls.type != event->type)
                continue;
#ifdef USE_ALLOCA
            new(copy + num) listenerbase(ls);
#else
            copy[num] = ls;
#endif

            ++num;
        }

        for (size_t i = 0; i != num; ++i)
        {
            listenerbase& ls = copy[i];
            //event->currentTarget = this;
            event->listenerID = ls.id;
            ls.cb(event);
            if (event->stopsImmediatePropagation)
                break;
        }

#ifdef USE_ALLOCA
        for (size_t i = 0; i != num; ++i)
        {
            listenerbase& ls = copy[i];
            ls.~listenerbase();
        }
#else
        delete[] copy;
#endif
    }

    template<class T>
    int EventDispatcherT<T>::getListenersCount() const
    {
        if (!_listeners)
            return 0;
        return (int)_listeners->size();
    }

    template class EventDispatcherT<Object>;
}