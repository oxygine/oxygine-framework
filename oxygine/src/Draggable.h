#pragma once
#include "Actor.h"
#include "Event.h"

namespace oxygine
{
	Vector2 convertPosUp(Actor *src, Actor *dest, const Vector2 &pos, bool direction);
	Vector2 convertPosDown(Actor *src, Actor *dest, const Vector2 &pos, bool direction);

	class Draggable
	{
	public:
		Draggable();
		~Draggable();


		void init(Actor *actor);
		void destroy();

		Actor*			getDragClient() const {return _dragClient;}
		const RectF&	getDragBounds() const {return _bounds;}

		bool getDragEnabled() const {return _dragEnabled;}
		void setDragEnabled(bool en){_dragEnabled = en;}

		/**sets bounds position for dragged actor*/
		void setDragBounds(const RectF &bounds);	
		/**sets destination drag client. Default value is Actor attached to DragHandler*/
		void setDragClient(Actor *actor);
		void snapClient2Bounds();
	protected:
		Actor *getClient();
		void onEvent(Event *event);

		void onDrag(TouchEvent *es);

		Actor *_dragClient;
		Actor *_actor;

		RectF _bounds;
		Vector2 _dragPos;
		Vector2 _clientPos;

		bool _clientIsParent;
		bool _pressed;
		bool _dragEnabled;
	};
}