#include "PointerState.h"
#include "Actor.h"
namespace oxygine
{
    PointerState::PointerState(): _index(0)
    {
        init(_index);
    }

    void PointerState::init(int pointerIndex)
    {
        _index = pointerIndex;
        _pressed = 0;
        _position.setZero();
    }

    bool PointerState::isPressed(MouseButton mb) const
    {
        return (_pressed & (1 << mb)) != 0;
    }
}