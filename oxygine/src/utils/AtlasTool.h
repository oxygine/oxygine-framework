#pragma once
#include "oxygine_include.h"
#include "math/Rect.h"
#include "AnimationFrame.h"
#include <deque>
#include <list>

namespace oxygine
{
    class AtlasNode
    {
    public:
        AtlasNode(AtlasNode* parent, const Rect& rect);
        ~AtlasNode();

        AtlasNode* insert(int width, int height);

        const Rect& getRect()const {return _rc;}
        void setID(int id) {_id = id;}

    private:
        AtlasNode* _parent;
        AtlasNode* _child[2];
        Rect _rc;

        int _id;
    };

    DECLARE_SMART(Texture, spTexture);

    class AnimationFrame;
    typedef std::vector<AnimationFrame> frames;

    class Atlas
    {
    public:
        Atlas();
        ~Atlas();

        void init(int w, int h);
        bool add(Texture* dest, const ImageData& src, Rect& srcRect);

        spTexture   getTexture();
        const Rect& getBounds() const {return _bounds;}

        void clean();

    private:
        Rect _bounds;
        AtlasNode* _tree;
    };



    class Atlas2
    {
    public:
        Atlas2();

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