#pragma once
#include "oxygine_include.h"
#include "Actor.h"
#include "core/coroutines.h"

namespace oxygine
{

	DECLARE_SMART(Tween, spTween);
	DECLARE_SMART(Clock, spClock);
	DECLARE_SMART(Button, spButton);
	namespace blocking
	{
		typedef int (*yieldCallback)();

		/** optional
			use default coroutines for yield or run your callback internally?
		*/
#if !OXYGINE_NO_YEILD
		void setYieldCallback(yieldCallback f);

		/**
			returns value from yieldCallback
		*/
		int yield();

		// schedule a coroutine to be resumed later
		void schedule(coroutine::handle fiber);

		// resume all scheduled coroutines
		void resumeAll();

		// wait for variable to become equal to value
		void waitValue(bool &variable, bool value);
		void waitTween(spTween);
		void waitTime(spClock clock, timeMS time);
		void waitTime(timeMS time);
		void waitClick(spActor, timeMS timeOut = 0);
#endif
	}
}