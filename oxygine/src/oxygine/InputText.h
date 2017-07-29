#pragma once
#include "oxygine-include.h"
#include "EventDispatcher.h"
#include "Event.h"
#include <string>

#ifndef __S3E__
#include "SDL.h"
#endif

namespace oxygine
{
    DECLARE_SMART(InputText, spInputText);

    class InputText: public EventDispatcher
    {
    public:
        enum {EVENT_TEXT_CHANGED = sysEventID('I', 'T', 'C') };
        enum {EVENT_COMPLETE = Event::COMPLETE};

        InputText();
        ~InputText();

        /**Shows virtual keyboard(if supported on platform) and sends pressed chars to TextField*/
        void start(spTextField textActor);
        void stop();

        void setAllowedSymbols(const std::string& utf8str);
        void setAllowedSymbols(const std::wstring& str);
        void setDisallowedSymbols(const std::string& utf8str);
        void setDisallowedSymbols(const std::wstring& str);
        void setMaxTextLength(int);

        static void stopAnyInput();

    private:
        spTextField _textActor;

        std::string _allowed;
        std::string _disallowed;
        std::string _txt;
        int _maxLength;
        void updateText();
        static InputText* _active;

        void _onSysEvent(Event* event);

#ifndef __S3E__
        int _onSDLEvent(SDL_Event* event);
#endif
    };
}