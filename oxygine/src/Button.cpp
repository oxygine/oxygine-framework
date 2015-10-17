#include "Button.h"
#include "Stage.h"

namespace oxygine
{
    void Button::copyFrom(const Button& src, cloneOptions opt)
    {
        _Sprite::copyFrom(src, opt);

        _state = src._state;
        _resAnim = src._resAnim;
        _row = src._row;
    }

    Button::Button(): _state(stateNormal), _resAnim(0), _row(0), _btnPressed(0), _btnOvered(0)
    {
        EventCallback ncb = CLOSURE(this, &Button::_mouseEvent);
        addEventListener(TouchEvent::TOUCH_DOWN, ncb);
        addEventListener(TouchEvent::OVER, ncb);
        addEventListener(TouchEvent::OUT, ncb);
        addEventListener(TouchEvent::CLICK, ncb);
    }

    Button::~Button()
    {
    }

    void Button::_mouseEvent(Event* event)
    {
        TouchEvent* me = safeCast<TouchEvent*>(event);

        switch (event->type)
        {
            case TouchEvent::CLICK:
            {
                event->phase = Event::phase_target;
                event->target = this;
            }
            break;
            case TouchEvent::OVER:
            {
                if (!_btnOvered)
                {
                    _btnOvered = me->index;
                    if (!_btnPressed)
                        setState(stateOvered);
                }
            }
            break;
            case TouchEvent::OUT:
            {
                if (_btnOvered == me->index)
                {
                    if (!_btnPressed)
                        setState(stateNormal);
                    _btnOvered = 0;
                }
            }
            break;
            case TouchEvent::TOUCH_DOWN:
            {
                if (!_btnPressed)
                {
                    _btnPressed = me->index;
                    setState(statePressed);

                    _getStage()->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Button::_mouseEvent));
                }
            }
            break;
            case TouchEvent::TOUCH_UP:
            {
                if (_btnPressed == me->index)
                {
                    setState(stateNormal);
                    if (_getStage())
                        _getStage()->removeEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Button::_mouseEvent));
                    _btnPressed = 0;
                }
            }
            break;
        }
    }

    void Button::setResAnim(const ResAnim* r, int, int)
    {
        _resAnim = r;
        updateButtonState(_state);
    }

    void Button::setRow(int row)
    {
        _row = row;
        updateButtonState(_state);
    }

    void Button::setState(state s)
    {
        if (s == _state)
            return;
        //printf("set state: %d\n", (int)s);
        _state = s;
        updateButtonState(s);
    }

    void Button::updateButtonState(state s)
    {
        if (!_resAnim)
            return;

        if (_resAnim->getColumns() > s)
            _Sprite::setAnimFrame(_resAnim->getFrame(s, _row));
        else
            _Sprite::setAnimFrame(_resAnim->getFrame(0, _row));

    }
}