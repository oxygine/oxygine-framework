#pragma once
#include "oxygine_include.h"
#include "SDL_keyboard.h"
namespace oxygine
{
    namespace key
    {
        typedef int keycode;

        void init();
        void release();

        bool wasPressed(keycode);
        bool wasReleased(keycode);

        /*could be used without key::init()**/
        bool isPressed(keycode);
    }
}