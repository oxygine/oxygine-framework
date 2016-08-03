#include "key.h"
#include "SDL.h"
#include <algorithm>

namespace oxygine
{
    namespace key
    {
        const int KEYS = 256;
        unsigned char _keys[KEYS];

        void update()
        {
            int num = 0;
            const Uint8* data = SDL_GetKeyboardState(&num);
            num = std::min(num, KEYS);
            memcpy(_keys, data, num);
        }


        bool wasPressed(keycode k)
        {
            const Uint8* data = SDL_GetKeyboardState(0);
            return data[k] && !_keys[k];
        }

        bool wasReleased(keycode key)
        {
            const Uint8* data = SDL_GetKeyboardState(0);
            return !data[key] && _keys[key];
        }

        bool isPressed(keycode k)
        {
            const Uint8* data = SDL_GetKeyboardState(0);
            return data[k] != 0;
        }
    }
}