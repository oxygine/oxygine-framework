#pragma once
#include "oxygine_include.h"
#include "math/vector2.h"
#include <list>

namespace oxygine
{
    class EventState;
    class Actor;

    enum MouseButton
    {
        MouseButton_Touch = 0,
        MouseButton_Left = 0,
        MouseButton_Middle = 1,
        MouseButton_Right = 2,
        MouseButton_Count
    };

    class PointerState
    {
    public:
        PointerState();

        void init(int pointerIndex);

        bool            isPressed(MouseButton mb = MouseButton_Touch) const {return _isPressed[mb];}
        int             getIndex() const {return _index;}
        const Vector2&  getPosition() const {return _position;}

    private:
        friend class Input;

        int _index;
        bool _isPressed[MouseButton_Count];
        Vector2 _position;
    };
}