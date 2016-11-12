#pragma once
#include "Event.h"
#undef OUT
namespace oxygine
{
    class PointerState;

    class TouchEvent : public Event
    {
    public:
        enum
        {
            __FIRST = sysEventID('T', 'O', 0),

            CLICK,
            OVER,
            OUT,
            MOVE,
            TOUCH_DOWN,
            TOUCH_UP,
            WHEEL_UP,
            WHEEL_DOWN,

            __LAST//system
        };


        TouchEvent(eventType type, bool Bubbles = true, const Vector2& locPosition = Vector2(0, 0)) : Event(type, Bubbles), localPosition(locPosition), position(locPosition), mouseButton(MouseButton_Touch), pressure(1.0f), index(1) {}

        Vector2 localPosition;//local position for Event::currentTarget actor
        Vector2 position;//local position for Event::target actor

        float pressure;

        MouseButton mouseButton;//valid only for TouchUP/Down events
        pointer_index index;

        const PointerState* getPointer() const;

        static bool isTouchEvent(int eventID) { return eventID > __FIRST && eventID < __LAST; }
    };
}