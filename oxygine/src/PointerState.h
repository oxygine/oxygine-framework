#pragma once
#include "oxygine_include.h"
#include "math/Vector2.h"
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
        MouseButton_Right = 2
    };

    class PointerState
    {
    public:
        PointerState();

        void init(int pointerIndex);

        bool            isPressed(MouseButton mb = MouseButton_Touch) const;
        int             getIndex() const {return _index;}
        const Vector2&  getPosition() const {return _position;}

    private:
        friend class Input;

        int _index;
        int _pressed;
        Vector2 _position;
    };
}
