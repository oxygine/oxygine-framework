#include "blocking.h"
//#include "Button.h"
#include "Tweener.h"
#include "Stage.h"

#if _MSC_VER
#define __func__ __FUNCTION__
#endif

namespace oxygine
{
	namespace blocking
	{	
		std::list<coroutine::handle> blocked;

		void schedule(coroutine::handle fiber)
		{
			blocked.push_back(fiber);
		}

		void resumeAll()
		{
			std::list<coroutine::handle> __blocked(blocked);
			blocked.clear();
			for (std::list<coroutine::handle>::iterator i = __blocked.begin(); i != __blocked.end(); ++i) {
				coroutine::handle y = *i;
				coroutine::resume(y);
				if (coroutine::isdead(y)) {
					coroutine::terminate(y);
				}                
			}
		}

		int default_resume()
		{
			schedule(coroutine::current());
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
			OX_ASSERT(_mainLoopFunc);
#if OXYGINE_NO_YEILD
			log::error("%s not supported", __func__);
			return 0;
#endif
			return _mainLoopFunc();
		}

		void waitValue(bool &variable, bool value)
		{
#if OXYGINE_NO_YEILD
			log::error("%s not supported", __func__);
			return;
#endif
			while (variable != value) {
				yield();
			}
	}

		void waitTween(spTween tween)
		{		
#if OXYGINE_NO_YEILD
			log::error("%s not supported", __func__);
			return;
#endif
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
#if OXYGINE_NO_YEILD
			log::error("%s not supported", __func__);
			return;
#endif

			timeMS start = clock->getTime();
			while(clock->getTime() - start < time)
			{
				yield();
			}
		}	

		void waitTime(timeMS time)
		{
#if OXYGINE_NO_YEILD
			log::error("%s not supported", __func__);
			return;
#endif
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
			timeMS _timeOut;

			void click(Event *ev)
			{
				_clicked = true;
			}

			clickWait(spActor button, timeMS timeOut):_clicked(false), _timeOut(timeOut)
			{
				spClock clock = getStage()->getClock();
				timeMS start = clock->getTime();
				button->addEventListener(TouchEvent::CLICK, CLOSURE(this, &clickWait::click));
				do
				{
					yield();
					if (timeOut > 0 && (clock->getTime() - start > timeOut))					
						break;
				} while (!_clicked);

				button->removeEventListeners(this);
			}
		};

		void waitClick(spActor button, timeMS timeOut)
		{
#if OXYGINE_NO_YEILD
			log::error("%s not supported", __func__);
			return;
#endif
			clickWait w(button, timeOut);			
		}
	}
}