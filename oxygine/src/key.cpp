#include "key.h"
#ifdef __S3E__
#   include "s3eKeyboard.h"
#else
#   include "SDL_keyboard.h"
#endif

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
#ifdef __S3E__
#else

            if (!_counter)
                return;

            int num = 0;
            const Uint8* data = SDL_GetKeyboardState(&num);
            num = std::min(num, KEYS);
            memcpy(_keys, data, num);
#endif
        }


        bool wasPressed(keycode key)
        {

#ifdef __S3E__
            return s3eKeyboardGetState((s3eKey)key) & S3E_KEY_STATE_PRESSED;
#else
            OX_ASSERT(_counter);
            const Uint8* data = SDL_GetKeyboardState(0);
            return data[key] && !_keys[key];
#endif
        }

        bool wasReleased(keycode key)
        {

#ifdef __S3E__
            return s3eKeyboardGetState((s3eKey)key) & S3E_KEY_STATE_RELEASED;
#else
            OX_ASSERT(_counter);
            const Uint8* data = SDL_GetKeyboardState(0);
            return !data[key] && _keys[key];
#endif
        }

        bool isPressed(keycode key)
        {

#ifdef __S3E__
            return s3eKeyboardGetState((s3eKey)key) & S3E_KEY_STATE_DOWN;
#else
            const Uint8* data = SDL_GetKeyboardState(0);
            return data[key] != 0;
#endif
        }
    }
}