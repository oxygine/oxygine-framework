#pragma once
#include "oxygine-include.h"
#include "math/Rect.h"
#include "AnimationFrame.h"
#include <deque>


namespace oxygine
{
    DECLARE_SMART(Texture, spTexture);

    class AnimationFrame;
    typedef std::vector<AnimationFrame> frames;
    
    class AtlasBuilder
    {
    public:
        AtlasBuilder();

        void init(int w, int h, int skipSize = 3);
        void clean();

        bool add(Texture* dest, const ImageData& src, Rect& srcRect, const Point& offset);

        const Rect& getBounds() const { return _bounds; }

    protected:
        typedef std::deque<Rect> rects;
        Rect _bounds;
        rects _free;
        int _skipSize;
    };

    typedef AtlasBuilder Atlas2;

    class MultiAtlas
    {
    public:
        typedef Closure<spTexture(int w, int h)> createTextureCallback;

        MultiAtlas(createTextureCallback);

        void init(int skipSize = 3);
        void clean();

        bool add(const ImageData& src, Rect& srcRect, spTexture& t);

        const Rect& getBounds() const { return _bounds; }

    protected:
        struct rect
        {
            spTexture texture;
            Rect rct;
        };

        createTextureCallback _cb;

        void place(const rect& dest, int w, int h, const ImageData& src, spTexture& t, Rect& srcRect);

        typedef std::deque<rect> rects;
        Rect _bounds;
        rects _free;
        int _skipSize;
        static bool sortRects(const rect& a, const rect& b);
    };
}