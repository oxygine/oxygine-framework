#pragma once
#include "math/vector3.h"
#include "math/vector2.h"
#include "closure/closure.h"
#include "PointerState.h"
#include "math/Rect.h"

namespace oxygine
{
	class Actor;
	class PointerState;
	class EventState;
	
	const int MAX_TOUCHES = 15;

	class Input
	{
	public:
		static Input instance;

		Input();
		~Input();

		void cleanup();

		/**id should be in range (1, MAX_TOUCHES)*/
		PointerState *getTouchByIndex(int index);

#ifndef __S3E__
		int	touchID2index(int id);
		PointerState *getTouchByID(int id);
#endif


		//internal:
		PointerState _pointers[MAX_TOUCHES];
		PointerState _pointerMouse;
		int _ids[MAX_TOUCHES];
		void sendPointerButtonEvent(MouseButton button, int x, int y, int type, PointerState *);
		void sendPointerMotionEvent(int x, int y, PointerState *);
		void sendPointerWheelEvent(int scroll, PointerState *);
	};
}