#pragma once
#include "Actor.h"

namespace oxygine
{
	DECLARE_SMART(RootActor, spRootActor);

	class DebugActor;

	class RootActor : public Actor
	{
	public:
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

		/**Returns original display size*/
		const Point &getDisplaySize() const {return _realDisplaySize;}
		//const Rect	&getArea();

		/**Updates each children*/
		void update();

		/**Passes event to each children*/
		//bool handleEvent(const EventState &es);

		/**thread safe way to pass event*/
		//void postEvent(const EventState &es);

		void cleanup();

		std::string dump(const dumpOptions &opt) const;

	protected:
		bool isOn(const Vector2 &localPosition);

	private:

		void safeRemove(spActor actor);

		Point _realDisplaySize;
		actors _removedActors;

		timeMS _statUpdate;
		timeMS _statRender;
		bool	_clipOuter;
		Rect	_viewport;

		friend class DebugActor;
	};

	inline spRootActor getRoot(){return RootActor::instance;}
}