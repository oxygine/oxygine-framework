#include "Button.h"
#include "Stage.h"

namespace oxygine
{
	void Button::copyFrom(const Button &src, cloneOptions opt)
	{
		Sprite::copyFrom(src, opt);

		_state = src._state;
		_resAnim = src._resAnim;
		_row = src._row;
	}

	Button::Button():_state(stateNormal), _resAnim(0), _row(0), _btnPressed(0), _btnOvered(0)
	{
		EventCallback ncb = CLOSURE(this, &Button::_mouseEvent);
		addEventListener(TouchEvent::TOUCH_DOWN, ncb);
		addEventListener(TouchEvent::OVER, ncb);
		addEventListener(TouchEvent::OUT, ncb);
		addEventListener(TouchEvent::CLICK, ncb);
	}

	Button::~Button()
	{
		/*
		removeEventHandlers();
		if (_ph)
		{
			_ph->reset();
		}
		*/
	}

	void Button::_mouseUpOutside(Event *event)
	{
		//Actor *act = safeCast<Actor*>(event->target.get());
		//if (isDescendant(act))
		_pressed = false;
		setState(stateNormal);
	}

	void Button::_mouseEvent(Event *event)
	{
		TouchEvent *me = safeCast<TouchEvent*>(event);
		Actor *act = safeCast<Actor*>(event->target.get());

		switch(event->type)
		{
		case TouchEvent::CLICK:
			{
				event->phase = Event::phase_target;
				event->target = this;
				/*
				if (event->target.get() != this)
				{
					spMouseEvent e = new MouseEvent(et_Click);
					dispatchEvent(e.get());
				}
				*/
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
				//if (isDescendant(act))
				//if (me->target.get() == this)
				{
					//event->stopImmediatePropagation();
					if (!_btnPressed)
					{
						_btnPressed = me->index;
						setState(statePressed);
						getStage()->addEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Button::_mouseEvent));
					}					
				}				
			}
			break;
		case TouchEvent::TOUCH_UP:
			{
				if (_btnPressed == me->index)
				{				
					setState(stateNormal);
					getStage()->removeEventListener(TouchEvent::TOUCH_UP, CLOSURE(this, &Button::_mouseEvent));
					_btnPressed = 0;
				}

				//if (isOn(me->localPosition))
				//if (isDescendant(act))
				if (_overed)
				{
					//spMouseEvent e = new MouseEvent(et_Click);
					//dispatchEvent(e.get());
				}
			}
			break;
		}		
	}

	void Button::setResAnim(const ResAnim *r)
	{
		_resAnim = r;
		updateButtonState(_state);
	}

	/*
	void Button::setAnimFrame(const ResAnim *resanim, int col, int row)
	{
		setResAnim(resanim);
	}
	*/

	void Button::setRow(int row)
	{
		_row = row;
		updateButtonState(_state);
	}
	/*
	void Button::setCallbackClick(EventCallback cb)
	{
		_cbClick = cb;
	}
	*/

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
			Sprite::setAnimFrame(_resAnim->getFrame(s, _row));
		else
			Sprite::setAnimFrame(_resAnim->getFrame(0, _row));
		
	}
}