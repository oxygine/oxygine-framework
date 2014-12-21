#pragma once
#include "Event.h"

namespace oxygine
{
	/**supported only on SDL*/
	class KeyEvent : public Event
	{
	public:
		enum KEY_EVENT {
			KEY_DOWN = sysEventID('K', 'E', 'D'),
			KEY_UP = sysEventID('K', 'E', 'U')
		};

		SDL_KeyboardEvent* data;
		KeyEvent(KEY_EVENT k, SDL_KeyboardEvent* d) :Event(k), data(d){}
	};
}