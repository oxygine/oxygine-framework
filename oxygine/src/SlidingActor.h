#pragma once
#include "oxygine_include.h"
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
		static void setDefaultTouchThreshold(float val);
		
		class SlidingEvent: public Event
		{
		public:
			enum SEvent
			{
				BEGIN = sysEventID('S', 'B', 'G'),
				SLIDING = sysEventID('S', 'S', 'L'),
				END = sysEventID('S', 'E', 'N')
			};

			SlidingEvent(SEvent ev) :Event(ev), speed(0, 0){}
			Vector2 speed;
		};


		SlidingActor();
		~SlidingActor();
	
		spActor			getContent() const {return _content;}
		const RectF&	getDragBounds() const {return _drag.getDragBounds();}

		/**max allowed radius of touch move when content could be clicked*/
		void setTouchThreshold(float rad);
		/**sets actor with scrollable data*/
		void setContent(spActor content);
		/**lock for user scrolling*/
		void setLocked(bool locked);
		/**snaps content*/
		void snap();
		/**stop if sliding*/
		void stop();

		void serialize(serializedata* data);
		void deserialize(const deserializedata* data);		

	protected:
		void destroy();

		void handleEvent(Event *event);

		void sizeChanged(const Vector2 &size);
		void doUpdate(const UpdateState &us);
		void updateDragBounds();
		

		void _newEvent(Event *event);

		bool _sliding;
		float _rad;
		float _maxSpeed;
		timeMS _downTime;

		Vector2 _downPos;
		Vector2 _speed;

		Draggable _drag;

		spActor _content;
		spClipRectActor _clip;

		spEventDispatcher _holded;

		struct  iter
		{
			Vector2 pos;
			timeMS tm;
		};

		timeMS _lastTime;
		enum {NUM = 11};
		iter _prev[NUM];
		int _current;

		timeMS _lastIterTime;
	};

}