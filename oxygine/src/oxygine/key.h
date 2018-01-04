#pragma once
#include "oxygine-include.h"
#include "SDL_keyboard.h"

namespace oxygine
{
    namespace key
    {
        typedef int keycode;

        void init();
        void release();

        /**
        * Indicates a key's state has changed from up to down in the last update.
        */
        bool wasPressed(keycode);

        /**
        * Indicates a key's state has changed from down to up in the last update.
        */
        bool wasReleased(keycode);


        /**
        * Indicates a key is currently pressed down.
        * could be used without key::init()
        */
        bool isPressed(keycode);
    }
}