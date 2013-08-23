#pragma once
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
		void setYieldCallback(yieldCallback f);

		/**
			returns value from yieldCallback
		*/
		int yield();

		void waitTween(spTween);
		void waitTime(spClock clock, timeMS time);
		void waitTime(timeMS time);
		void waitClick(spActor);
	}
}