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

	DECLARE_SMART(TextField, spTextField);
	DECLARE_SMART(InputText, spInputText);

	class InputText: public EventDispatcher
	{
	public:
		InputText();
		~InputText();

		/**Shows virtual keyboard(if supported on platform) and sends pressed chars to TextField*/
		void start(spTextField textActor);
		void stop();

		void setAllowedSymbols(const string &utf8str);
		void setAllowedSymbols(const wstring &str);
		void setDisallowedSymbols(const string &utf8str);
		void setDisallowedSymbols(const wstring &str);
		void setMaxTextLength(int);

		static void stopAnyInput();

	private:
		spTextField _textActor;

		string _allowed;
		string _disallowed;
		string _txt;
		int	_maxLength;
		void updateText();
		static InputText *_active;		

		void _onPlatform(Event *event);

#ifndef __S3E__
		int _onSDLEvent(SDL_Event *event);
#endif
	};
}