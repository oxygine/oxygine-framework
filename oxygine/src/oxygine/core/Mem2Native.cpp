#include "Mem2Native.h"
#include "ImageDataOperations.h"

namespace oxygine
{
    const int RECT_SIZE = 4096;

    Mem2Native::Mem2Native(): _prev(0, 0), _size(0), _opt(0)
    {
    }

    Mem2Native::~Mem2Native()
    {
    }

    void Mem2Native::push(const CreateTextureTask& opt)
    {
        //copy should be deleted later
        CreateTextureTask* copy = new CreateTextureTask(opt);
        _messages.send(0, copy, 0);
    }

    bool Mem2Native::isEmpty()
    {
        return _prev == Point(0, 0) && _messages.empty();
    }

    void Mem2Native::textureDone()
    {
        _opt->ready();
        delete _opt;
        _opt = 0;
    }

    void Mem2Native::updateTexture()
    {
        int SIZE = _size;
        if (!SIZE)
            SIZE = RECT_SIZE;

        Image* src = _opt->src.get();
        NativeTexture* dest = _opt->dest.get();
        Point& prev = _prev;

        bool done = false;

        if (isCompressedFormat(src->getFormat()))
        {
            dest->init(src->lock(), false);
            done = true;
        }
        else
        {

            Rect textureRect(0, 0, src->getWidth(), src->getHeight());

            if (dest->getHandle() == 0)
                dest->init(textureRect.getWidth(), textureRect.getHeight(), src->getFormat());


            Rect srcRect(prev.x, prev.y,
                         std::min(SIZE, textureRect.getWidth()), std::min(SIZE, textureRect.getHeight()));
            srcRect.clip(textureRect);


            ImageData srcim = src->lock(&srcRect);


            ImageData destim;
            if (srcRect != textureRect)
            {
                int pitch = srcRect.getWidth() * getBytesPerPixel(dest->getFormat());
                _buffer.resize(srcRect.getHeight() * pitch);
                destim = ImageData(
                             srcRect.getWidth(), srcRect.getHeight(),
                             pitch,
                             dest->getFormat(),
                             &_buffer[0]
                         );

                operations::copy(srcim, destim);
            }
            else
            {
                destim = srcim;
            }

            dest->updateRegion(srcRect.pos.x, srcRect.pos.y, destim);

            prev.x += SIZE;
            if (prev.x >= textureRect.getWidth())
            {
                prev.x = 0;
                prev.y += SIZE;
            }

            if (prev.y >= textureRect.getBottom())
            {
                _buffer.clear();
                prev = Point(0, 0);
                done = true;
            }
        }

        if (done)
        {
            textureDone();
        }
    }

    void Mem2Native::update()
    {
        //update only one texture per frame
        if (_prev == Point(0, 0))
        {
            ThreadDispatcher::peekMessage ev;
            if (_messages.peek(ev, true))
            {
                CreateTextureTask* opt = (CreateTextureTask*)ev.arg1;
                _opt = opt;

                updateTexture();
            }
        }
        else
        {
            updateTexture();
        }
    }
}