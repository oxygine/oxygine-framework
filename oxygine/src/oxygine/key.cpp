#include "key.h"
#include "SDL_keyboard.h"

#include <algorithm>

namespace oxygine
{
    namespace key
    {
        int _counter = 0;

        const int KEYS = 256;
        unsigned char _keys[KEYS];

        void init()
        {
            if (!_counter)
                memset(_keys, 0, sizeof(_keys));

            _counter++;
        }

        void release()
        {
            _counter--;
            OX_ASSERT(_counter >= 0);
        }

        void update()
        {
            if (!_counter)
                return;

            int num = 0;
            const Uint8* data = SDL_GetKeyboardState(&num);
            num = std::min(num, KEYS);
            memcpy(_keys, data, num);
        }


        bool wasPressed(keycode key)
        {
            OX_ASSERT(_counter);
            const Uint8* data = SDL_GetKeyboardState(0);
            return data[key] && !_keys[key];
        }

        bool wasReleased(keycode key)
        {
            OX_ASSERT(_counter);
            const Uint8* data = SDL_GetKeyboardState(0);
            return !data[key] && _keys[key];
        }

        bool isPressed(keycode key)
        {
            const Uint8* data = SDL_GetKeyboardState(0);
            return data[key] != 0;
        }
    }
}