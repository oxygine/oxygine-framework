#include "PointerState.h"
#include "Actor.h"
namespace oxygine
{
    PointerState::PointerState(): _index(1)
    {
        init(_index);
    }

    void PointerState::init(pointer_index index)
    {
        _index = index;
        _pressed = 0;
        _position.setZero();
    }

    bool PointerState::isPressed(MouseButton mb) const
    {
        return (_pressed & (1 << mb)) != 0;
    }
}