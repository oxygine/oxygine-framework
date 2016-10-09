#pragma once
#include "oxygine_include.h"
#include "ref_counter.h"
#include <vector>
#include <list>
#include "NativeTexture.h"
#include "Image.h"
#include "Mutex.h"
#include "ThreadDispatcher.h"
#include "res/CreateResourceContext.h"

namespace oxygine
{
    DECLARE_SMART(Image, spImage);
    DECLARE_SMART(NativeTexture, spNativeTexture);

    class Mem2Native
    {
    public:
        Mem2Native();
        ~Mem2Native();

        bool isEmpty();

        void push(const CreateTextureTask& opt);
        void update();

        void setUpdateSize(int size) {_size = size;}

    private:

        void updateTexture();
        void textureDone();

        int _size;
        Point _prev;

        CreateTextureTask* _opt;

        std::vector<unsigned char> _buffer;
        ThreadDispatcher _messages;//todo, use global messages
    };
}
