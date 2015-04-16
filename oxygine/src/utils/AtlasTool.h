#pragma once
#include "oxygine_include.h"
#include "math/Rect.h"
#include "AnimationFrame.h"

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
}