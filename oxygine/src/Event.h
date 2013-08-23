#pragma once
#include <list>
#include "core/Object.h"
#include "closure/closure.h"
#include "Input.h"
#include "EventDispatcher.h"

namespace oxygine
{

	enum events
	{
		_et_unknown,
		_et_TouchFirst,
		_et_TouchDown,
		_et_TouchUp,
		_et_TouchMove,
		_et_TouchClick,
		_et_TouchOver,
		_et_TouchOut,
		
		_et_WheelUp,
		_et_WheelDown,

		_et_TouchLast,

		_et_TweenDone,

		_et_Complete,
		//_et_RollOver,
		//_et_RollOut,
		

		_et_maxEvent = 0xFFFFFFFF
	};



	DECLARE_SMART(Actor, spActor);

	typedef char pointer_index;
	class PointerState;

	class TouchEvent: public Event
	{
	public:
		enum 
		{
			CLICK = _et_TouchClick,
			OVER = _et_TouchOver,
			OUT = _et_TouchOut,
			MOVE = _et_TouchMove,
			TOUCH_DOWN = _et_TouchDown,
			TOUCH_UP = _et_TouchUp,
			
			WHEEL_UP = _et_WheelUp,
			WHEEL_DOWN = _et_WheelDown,
		};


		TouchEvent(eventType type, bool Bubbles = true, const Vector2 &locPosition = Vector2(0, 0)):Event(type, Bubbles), localPosition(locPosition), position(locPosition), mouseButton(MouseButton_Touch){}
		Vector2 localPosition;
		Vector2 position;

		const PointerState *getPointer() const;

		MouseButton mouseButton;//valid only for TouchUP/Down

		pointer_index index;
	};

}