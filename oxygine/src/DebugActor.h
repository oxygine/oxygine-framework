#pragma once
#include "oxygine_include.h"
#include "core/oxygine.h"
#include "res/ResFontBM.h"
#include "Actor.h"


namespace oxygine
{
	class ResFontBM;
	class ResAnim;
	class Resources;

	DECLARE_SMART(TextActor, spTextActor);
	DECLARE_SMART(ColorRectSprite, spColorRectSprite);
	DECLARE_SMART(DebugActor, spDebugActor);

	/**DebugActor is helper class. It displays FPS, memory usage, other stats and dev tools buttons*/
	class DebugActor: public Actor
	{
	public:
		static spDebugActor instance;

		static Resources* resSystem;
		static void initialize();
		static void release();
		static string getDefaultName(){return "debug_actor";}

		DebugActor();
		~DebugActor();

		void setCornerPosition(int corner);
		//void addDebugString(const string &str);
		void addDebugString(const char *format, ...);

		/**function for debug. Helps you to find actor who handled TouchEvent*/
		void showTouchedActor(bool show);
		void showTexel2PixelErrors(bool show);

	protected:
		void showDevMenu(spActor data);
		void _btnClicked(Event *ev);
		

		void doUpdate(const UpdateState &us);
		void render(RenderState const& parentRenderState);
		void addButton(float &x, const char *name, const char *anim);
	private:
		
		spColorRectSprite _bg;
		spTextActor _text;
		string _debugText;
		int _frames;
		timeMS _startTime;
		int _corner;

		bool _showTouchedActor;
		bool _showTexel2PixelErrors;

		void onEvent(Event *ev);
		void onDAEvent(Event *ev);
	};
}