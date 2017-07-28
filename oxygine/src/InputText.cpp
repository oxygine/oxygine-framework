#include "InputText.h"
#include "TextField.h"
#include "utils/stringUtils.h"
#include "core/oxygine.h"

#ifndef __S3E__
#include "SDL_keyboard.h"
#include "SDL_events.h"
#endif

namespace oxygine
{
    InputText* InputText::_active = 0;

    InputText::InputText(): _maxLength(0)
    {

    }

    InputText::~InputText()
    {

    }

    void InputText::stopAnyInput()
    {
        if (!_active)
            return;
        _active->stop();
        _active = 0;
    }

    void InputText::start(spTextField ta)
    {
#ifdef __S3E__
        log::error("InputText isn't implemented for MARMALADE");
#endif

        addRef();

        //if (_active == this)
        //  return;
        if (_active)
        {
            _active->stop();
        }

        _active = this;

        _textActor = ta;

        core::getDispatcher()->addEventListener(core::EVENT_SYSTEM, CLOSURE(this, &InputText::_onSysEvent));

#ifndef __S3E__
        SDL_StartTextInput();
#endif
        //log::messageln("InputText::start  %x", this);
        _txt = "";
        updateText();
    }

    void InputText::setAllowedSymbols(const std::string& utf8str)
    {
        _allowed = utf8str;
    }

    void InputText::setAllowedSymbols(const std::wstring& str)
    {
        _allowed = ws2utf8(str.c_str());
    }

    void InputText::setDisallowedSymbols(const std::string& utf8str)
    {
        _disallowed = utf8str;
    }

    void InputText::setDisallowedSymbols(const std::wstring& str)
    {
        _disallowed = ws2utf8(str.c_str());
    }

    void InputText::setMaxTextLength(int v)
    {
        _maxLength = v;
    }

    void InputText::stop()
    {
#ifndef __S3E__
        SDL_StopTextInput();
#endif
        core::getDispatcher()->removeEventListeners(this);

        _active = 0;
        _textActor = 0;
        //log::messageln("InputText::stop  %x", this);
        releaseRef();
    }

    void InputText::_onSysEvent(Event* event)
    {
#ifndef __S3E__
        _onSDLEvent((SDL_Event*)event->userData);
#endif
    }

    void InputText::updateText()
    {
        _textActor->setText(_txt);

        Event evnt(EVENT_TEXT_CHANGED);
        dispatchEvent(&evnt);
    }

    bool findCode(const char* str, int c)
    {
        while (*str)
        {
            int code = 0;
            str = getNextCode(code, str);
            if (code == c)
                return true;
        }
        return false;
    }

    int getLen(const char* str)
    {
        int i = 0;
        while (*str)
        {
            ++i;
            int code = 0;
            str = getNextCode(code, str);
        }
        return i;
    }

    int getLastPos(const char* str)
    {
        const char* begin = str;
        const char* prev = str;
        while (*str)
        {
            prev = str;
            int code = 0;
            str = getNextCode(code, str);
        }
        return (int)(prev - begin);
    }

#ifndef __S3E__
    int InputText::_onSDLEvent(SDL_Event* event)
    {
        switch (event->type)
        {
            case SDL_TEXTEDITING:
            {
                //SDL_TextEditingEvent &te = event->edit;
                //int q=0;
            }
            break;
            case SDL_TEXTINPUT:
            {
                if (_maxLength)
                {
                    if (getLen(_txt.c_str()) >= _maxLength)
                        return 0;
                }

                SDL_TextInputEvent& te = event->text;
                //log::messageln("text: %d %d %d %d", (int)(te.text[0]), (int)(te.text[1]), (int)(te.text[2]), (int)(te.text[3]));

                int newCode = 0;
                getNextCode(newCode, te.text);

                if (!_disallowed.empty())
                {
                    if (findCode(_disallowed.c_str(), newCode))
                        newCode = 0;
                }

                if (!newCode)
                    return 0;

                if (!_allowed.empty())
                {
                    if (!findCode(_allowed.c_str(), newCode))
                        newCode = 0;
                }

                if (!newCode)
                    return 0;

                _txt.append(te.text, te.text + strlen(te.text));
                updateText();
            }
            break;
            case SDL_KEYDOWN:
            {
                //log::messageln("SDL_KEYDOWN");
                switch (event->key.keysym.sym)
                {
                    case SDLK_BACKSPACE:
                    {
                        if (!_txt.empty())
                        {
                            int pos = getLastPos(_txt.c_str());
                            _txt.erase(_txt.begin() + pos, _txt.end());
                        }
                        updateText();
                    }
                    break;
                    case SDLK_RETURN:
                    {
                        Event evnt(EVENT_COMPLETE);
                        dispatchEvent(&evnt);
                    }
                    break;
                }
            }
            break;
        }
        return 0;
    }
#endif
}