#pragma once
#include "oxygine-include.h"
#include "EventDispatcher.h"
#include "PointerState.h"
#include "closure/closure.h"

namespace oxygine
{
    const int MAX_TOUCHES = 17;

    class Input
    {
    public:
        static Input instance;

        Input();
        ~Input();

        void cleanup();

        void multiTouchEnabled(bool en);

        /**index should be in range [1, MAX_TOUCHES]*/
        PointerState* getTouchByIndex(pointer_index index);

#ifndef __S3E__
        int touchID2index(int64 id);
        PointerState* getTouchByID(int64 id);
#endif


        //internal:
        PointerState _pointers[MAX_TOUCHES];
        PointerState _pointerMouse;

        int64 _ids[MAX_TOUCHES + 1];


        void sendPointerButtonEvent(spStage, MouseButton button, float x, float y, float pressure, int type, PointerState*);
        void sendPointerMotionEvent(spStage, float x, float y, float pressure, PointerState*);
        void sendPointerWheelEvent(spStage, const Vector2& dir, PointerState*);

        bool _multiTouch;
    };
}
