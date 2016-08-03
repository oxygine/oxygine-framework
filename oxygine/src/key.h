#pragma once
#include "oxygine_include.h"
#include "SDL_keyboard.h"
namespace oxygine
{
    namespace key
    {
        /**should be called before core::update*/
        void update();

        typedef int keycode;

        bool wasPressed(keycode);
        bool wasReleased(keycode);
        bool isPressed(keycode);
    }
}