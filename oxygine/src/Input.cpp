#include "Input.h"
#include "Stage.h"
#include "core/log.h"
#include <string.h>

//#define LOGD(...) oxygine::log::message("input: "); oxygine::log::messageln(__VA_ARGS__)
#define LOGD(...) ((void)0)

namespace oxygine
{
    Input Input::instance;

    const PointerState* TouchEvent::getPointer() const
    {
        return Input::instance.getTouchByIndex(index);
    }

    void Input::sendPointerButtonEvent(spStage stage, MouseButton button, float x, float y, float pressure, int type, PointerState* ps)
    {
        Vector2 p(x, y);

        TouchEvent me(type, true, p);
        me.index = ps->getIndex();
        me.mouseButton = button;
        me.pressure = pressure;

        if (type == TouchEvent::TOUCH_DOWN)
            ps->_isPressed[button] = true;
        else if (type == TouchEvent::TOUCH_UP)
            ps->_isPressed[button] = false;

        ps->_position = p;

        LOGD("sendPointerButtonEvent %d - (%.2f, %.2f), %d", me.index, p.x, p.y, type);

        stage->handleEvent(&me);

        if (type == TouchEvent::TOUCH_UP)
        {
            // Remove id and compact the array:
            //  - stop on first copied zero id
            //  - if end is reached, zero the last element
            int i = ps->getIndex() - 1;
            while (i < MAX_TOUCHES - 1)
            {
                if ((_ids[i] = _ids[i + 1]) == 0)
                    break;
                _pointers[i] = _pointers[i + 1];
                _pointers[i]._index = i + 1;
                ++i;
            }
            if (i == MAX_TOUCHES - 1)
            {
                _ids[i] = 0;
                _pointers[i].init(i + 1);
            }
        }
    }

    void Input::sendPointerMotionEvent(spStage stage, float x, float y, float pressure, PointerState* ps)
    {
        TouchEvent me(TouchEvent::MOVE, true, Vector2(x, y));
        me.index = ps->getIndex();
        me.pressure = pressure;
        ps->_position = Vector2(x, y);

        LOGD("sendPointerMotionEvent %d - (%.2f, %.2f)", me.index, x, y);
        stage->handleEvent(&me);
    }

    void Input::sendPointerWheelEvent(spStage stage, int scroll, PointerState* ps)
    {
        TouchEvent me(scroll > 0 ? TouchEvent::WHEEL_UP : TouchEvent::WHEEL_DOWN, true);
        me.index = ps->getIndex();

        ps->_position = Vector2(0, 0);

        stage->handleEvent(&me);
    }


    Input::Input()
    {
        addRef();
        _pointerMouse.init(MAX_TOUCHES + 1);
        for (int i = 0; i < MAX_TOUCHES; ++i)
            _pointers[i].init(i + 1);
        memset(_ids, 0, sizeof(_ids));
    }

    Input::~Input()
    {

    }

    void Input::cleanup()
    {
    }

    PointerState* Input::getTouchByIndex(int index)
    {
        if (index == MAX_TOUCHES + 1)
            return &_pointerMouse;
        index -= 1;
        OX_ASSERT(index >= 0 && index < MAX_TOUCHES);
        index = std::min(std::max(index, 0), MAX_TOUCHES);
        return &_pointers[index];
    }

#ifndef __S3E__
    int Input::touchID2index(int64_t id)
    {
        id += 1;//id could be = 0 ?
        for (int i = 0; i < MAX_TOUCHES; ++i)
        {
            int64_t& d = _ids[i];
            int index = i + 1;
            if (d == 0)
            {
                d = id;
                return index;
            }

            if (d == id)
                return index;
        }
        //log::warning("can't find touch id %d", id);
        return -1;
    }

    PointerState* Input::getTouchByID(int64_t id)
    {
        int i = touchID2index(id);
        if (i == -1)
            return 0;
        return getTouchByIndex(i);
    }
#endif
}