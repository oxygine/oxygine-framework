#pragma once
#include "oxygine_include.h"
#include "Actor.h"

namespace oxygine
{
	DECLARE_SMART(RootActor, spRootActor);

	class DebugActor;
	class RootActor : public Actor
	{
	public:
		enum
		{
			ACTIVATE = makefourcc('_', 'R', 'A', 'c'),
			DEACTIVATE = makefourcc('_', 'R', 'D', 'c'),
			LOST_CONTEXT = makefourcc('_', 'R', 'L', 'C'),
		};

		static spRootActor instance;
		static Rect	calcCenteredViewport(const Point &displaySize, const Point &gameSize);

		RootActor();
		~RootActor();

		/**Initializes size, scale and position of RootActor 
		@param is real display size of device.
		@param is your "virtual" preferred size. You could change it to any size you need
		*/
		void init(const Point &displaySize, const Point &gameSize);
				
		void setClipOuterArea(bool clip){_clipOuter = clip;}
		/**Renders each children*/
		void render(Renderer &r);

		//const Rect	&getArea();

		/**Updates each children*/
		void update();

		void cleanup();

		RectF getDestRect() const;

		std::string dump(const dumpOptions &opt) const;

	protected:
		bool isOn(const Vector2 &localPosition);

	private:

		timeMS _statUpdate;
		timeMS _statRender;
		bool	_clipOuter;
		Rect	_viewport;

		friend class DebugActor;
	};

	inline spRootActor getRoot(){return RootActor::instance;}
}