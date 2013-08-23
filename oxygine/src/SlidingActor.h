#pragma once
#include "Actor.h"
#include "math/vector2.h"
#include "Draggable.h"
#include "ClipRectActor.h"

namespace oxygine
{
	DECLARE_SMART(SlidingActor, spSlidingActor);
	DECLARE_SMART(DragHandler, spDragHandler);

	class SlidingActor: public Actor
	{
	public:
		SlidingActor();
		~SlidingActor();
			
		spActor			getContent() const {return _content;}
		const RectF&	getDragBounds() const {return _drag.getDragBounds();}

		/**max allowed radius of touch move when content could be clicked*/
		void setTouchThreshold(float rad);
		void setContent(spActor content);
		void setSnapPageSize(const Vector2 &size);
		/**how fast page would be snapped, default value = 1.0f*/
		void setSnapSpeed(float v){_snapSpeed = v;}

		void snap();		

		void setCallbackSlideDone(EventCallback cb){_cbSlideDone = cb;}

	protected:
		void destroy();

		void handleEvent(Event *event);

		void sizeChanged(const Vector2 &size);
		void doUpdate(const UpdateState &us);
		void updateDragBounds();
		
		/*
		bool onMouseDown(const EventState &es);
		bool onMouseUp(const EventState &es);
		bool onMotion(const EventState &es);
		bool onDrag(const EventState &es);

		void deactivate(const EventState &es);
		*/
		//void slideDone();

		void _newEvent(Event *event);
		Vector2 _snapSize;

		bool _movingX;
		bool _movingY;

		bool _sliding;
		float _rad;
		float _snapSpeed;
		float _maxSpeed;
		timeMS _downTime;

		Vector2 _downPos;
		Vector2 _speed;
		Vector2 _prevPos;

		Draggable _drag;

		spActor _content;
		spClipRectActor _clip;
		//spDragHandler _slideOnTopDH;
		EventCallback _cbSlideDone;

		spEventDispatcher _holded;
		bool	_down;
	};

}