#pragma once
#include "oxygine_include.h"
#include "ref_counter.h"
#include <vector>
#include <list>
#include "NativeTexture.h"
#include "MemoryTexture.h"
#include "Mutex.h"
#include "ThreadMessages.h"

namespace oxygine
{
    DECLARE_SMART(MemoryTexture, spMemoryTexture);
    DECLARE_SMART(NativeTexture, spNativeTexture);

    class Mem2Native
    {
    public:
        Mem2Native();
        ~Mem2Native();

        bool isEmpty();

        void push(spMemoryTexture src, spNativeTexture dest);
        void update();

        void setUpdateSize(int size) {_size = size;}

    private:

        void updateTexture(Point& prev, MemoryTexture* src, NativeTexture* dest);

        int _size;
        Point _prev;

        MemoryTexture* _src;
        NativeTexture* _dest;

        std::vector<unsigned char> _buffer;
        ThreadMessages _messages;//todo, use global messages
    };
}
