#pragma once
#include "oxygine_include.h"
#include "EventDispatcher.h"
#include <string>

#ifndef __S3E__
#include "SDL.h"
#endif

namespace oxygine
{
	using namespace std;

	DECLARE_SMART(TextActor, spTextActor);
	DECLARE_SMART(InputText, spInputText);

	class InputText: public EventDispatcher
	{
	public:
		/**Shows virtual keyboard(if supported on platform) and sends pressed chars to TextActor*/
		void start(spTextActor textActor);
		void stop();

		void setAllowedSymbols(const string &utf8str);
		void setAllowedSymbols(const wstring &str);
		void setDisallowedSymbols(const string &utf8str);
		void setDisallowedSymbols(const wstring &str);

		static void stopAnyInput();

	private:
		spTextActor _textActor;

		string _allowed;
		string _disallowed;
		string _txt;
		void updateText();
		static InputText *_active;		

#ifndef __S3E__		
		static int SDLCALL onSDLEvent(void *userdata, SDL_Event * event);
		int _onSDLEvent(SDL_Event *event);
#endif
	};
}