#pragma once
#include "oxygine-include.h"
#include "Actor.h"
//#include "Event.h"

namespace oxygine
{
    class TouchEvent;
    Vector2 convertPosUp(Actor* src, Actor* dest, const Vector2& pos, bool direction);
    Vector2 convertPosDown(Actor* src, Actor* dest, const Vector2& pos, bool direction);

    class Draggable
    {
    public:
        Draggable();
        ~Draggable();

        void init(Actor* actor);
        void start(const PointerState* pointer, Actor* actor, const Vector2& localCenter);

        void destroy();

        Actor*          getDragClient() const {return _dragClient;}
        const RectF&    getDragBounds() const {return _bounds;}
        const Vector2&  getClientPos() const {return _clientPos;}
        bool            getDragEnabled() const { return _dragEnabled; }
        bool            isDragging() const {return _pressed;}

        void setDragEnabled(bool en) {_dragEnabled = en;}
        /**sets bounds position for dragged actor*/
        void setDragBounds(const RectF& bounds);
        /**sets destination drag client. Default value is Actor attached to DragHandler*/
        void setDragClient(Actor* actor);
        void setIgnoreTouchUp(bool ignore) { _ignoreTouchUp = ignore; }
        void snapClient2Bounds();

    protected:
        Actor* getClient();
        void onEvent(Event* event);
        void startDrag(const Vector2& localPos);

        void onDrag(TouchEvent* es);
        void onMove(const Vector2& position);

        Actor* _dragClient;
        Actor* _actor;
        timeMS _startTm;
        bool _singleDrag;

        RectF _bounds;
        Vector2 _dragPos;
        Vector2 _clientPos;

        bool _clientIsParent;
        bool _pressed;
        bool _dragEnabled;
        bool _ignoreTouchUp;
    };
}
