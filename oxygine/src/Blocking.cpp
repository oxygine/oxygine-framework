#include "Blocking.h"
#include "Button.h"
#include "Tweener.h"

namespace oxygine
{
	namespace blocking
	{	
		int default_resume()
		{
			coroutine::resume();
			return 0;
		}

		yieldCallback _mainLoopFunc = default_resume;
		void setYieldCallback(yieldCallback f)
		{
			_mainLoopFunc = f;
		}	

		int yield()
		{
			return _mainLoopFunc();
		}

		void waitTween(spTween tween)
		{		
			if (tween->getDelay() == 0)//todo, workaround, fix assert for tweens with delay
			{
				OX_ASSERT(tween->isStarted());
			}
			
			while (!tween->isDone())
			{
				yield();		
			}
		}

		void waitTime(spClock clock, timeMS time)
		{
			timeMS start = clock->getTime();
			while(clock->getTime() - start < time)
			{
				yield();
			}
		}	

		void waitTime(timeMS time)
		{
			timeMS start = getTimeMS();
			while(getTimeMS() - start < time)
			{
				yield();
			}
		}


		class clickWait
		{
		public:
			bool _clicked;

			void click(Event *ev)
			{
				_clicked = true;
			}

			clickWait(spActor button):_clicked(false)
			{
				button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &clickWait::click));
				do
				{
					yield();
				} while (!_clicked);

				button->removeEventListeners(this);
			}
		};

		void waitClick(spActor button)
		{
			clickWait w(button);			
		}
	}
}