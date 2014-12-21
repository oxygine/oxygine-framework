#pragma once
#include "oxygine_include.h"
#include "Actor.h"

namespace oxygine
{
	DECLARE_SMART(Stage, spStage);

	class DebugActor;
	class Stage : public Actor
	{
	public:
		enum
		{
			ACTIVATE = sysEventID('S', 'A', 'c'),
			DEACTIVATE = sysEventID('S', 'D', 'c'),
			LOST_CONTEXT = sysEventID('S', 'L', 'C'),
		};

		static spStage instance;
		static Rect	calcCenteredViewport(const Point &displaySize, const Point &gameSize);

		Stage(bool autoHandleReset = false);
		~Stage();

		/**Initializes size, scale and position of Stage 
		@param is real display size of device.
		@param is your "virtual" preferred size. You could change it to any size you need
		*/
		void init(const Point &displaySize, const Point &gameSize);
				
		/**use Scissor test for outer Stage area*/
		void setClipOuterArea(bool clip){_clipOuter = clip;}
		/**Render all actors*/
		void render(Renderer &r);
		/**Render all actors, simplified version*/
		void render(const Color &clearColor, const Rect &viewport);

		//const Rect	&getArea();

		/**Updates each children*/
		void update();

		void cleanup();

		RectF getDestRect() const;


		std::string dump(const dumpOptions &opt) const;

	protected:
		void onDeactivate(Event *);
		void onActivate(Event *);

		//bool _active;

		bool isOn(const Vector2 &localPosition);

	private:

		timeMS _statUpdate;
		timeMS _statRender;
		bool	_clipOuter;
		Rect	_viewport;

		friend class DebugActor;
	};

	inline const spStage& getStage() {return Stage::instance;}
}