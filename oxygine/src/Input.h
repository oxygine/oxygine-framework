#pragma once
#include "oxygine_include.h"
#include "EventDispatcher.h"
#include "closure/closure.h"
#include "PointerState.h"

namespace oxygine
{
	class Actor;
	class EventState;
	
	const int MAX_TOUCHES = 8;

	class Input: public EventDispatcher
	{
	public:
		static Input instance;

		Input();
		~Input();

		enum
		{
			event_platform = sysEventID('I', 'P', 'L')
		};

		void cleanup();

		/**id should be in range [1, MAX_TOUCHES]*/
		PointerState *getTouchByIndex(int index);

#ifndef __S3E__
		int	touchID2index(int id);
		PointerState *getTouchByID(int id);
#endif


		//internal:
		PointerState _pointers[MAX_TOUCHES];
		PointerState _pointerMouse;

		int _ids[MAX_TOUCHES + 1];


		void sendPointerButtonEvent(MouseButton button, float x, float y, float pressure, int type, PointerState *);
		void sendPointerMotionEvent(float x, float y, float pressure, PointerState *);
		void sendPointerWheelEvent(int scroll, PointerState *);
	};
}