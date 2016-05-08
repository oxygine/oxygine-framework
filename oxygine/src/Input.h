#pragma once
#include "oxygine_include.h"
#include "EventDispatcher.h"
#include "closure/closure.h"
#include "PointerState.h"

namespace oxygine
{
    DECLARE_SMART(Stage, spStage);
    class Actor;
    class EventState;

    const int MAX_TOUCHES = 17;

    class Input: public Object
    {
    public:
        static Input instance;

        Input();
        ~Input();

        void cleanup();

        /**id should be in range [1, MAX_TOUCHES]*/
        PointerState* getTouchByIndex(int index);

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
        void sendPointerWheelEvent(spStage, int scroll, PointerState*);
    };
}