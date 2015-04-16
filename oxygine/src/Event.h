#pragma once
#include "oxygine_include.h"
#include "closure/closure.h"
#include "Input.h"
#include "EventDispatcher.h"
#undef OUT

struct SDL_KeyboardEvent;

namespace oxygine
{
    typedef int eventType;
    DECLARE_SMART(EventDispatcher, spEventDispatcher);

    class Event
    {
    public:
        enum { COMPLETE = sysEventID('C', 'M', 'P') };

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

        void* userData;
        spObject userDataObject;

        spEventDispatcher target;
        spEventDispatcher currentTarget;// = object with our listener

        Event(eventType Type, bool Bubbles = false) : userData(0), type(Type), phase(phase_target), bubbles(Bubbles), stopsImmediatePropagation(false), stopsPropagation(false) {}
        virtual ~Event() {}

        void stopPropagation() { stopsPropagation = true; }
        void stopImmediatePropagation() { stopsPropagation = stopsImmediatePropagation = true; }
    };
}
