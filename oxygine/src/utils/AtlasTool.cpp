#include "AtlasTool.h"
#include "core/Texture.h"

namespace oxygine
{
    AtlasNode::AtlasNode(AtlasNode* parent, const Rect& rect): _parent(parent), _rc(rect), _id(0)
    {
        _child[0] = 0;
        _child[1] = 0;
    }

    AtlasNode::~AtlasNode()
    {
        delete _child[0];
        delete _child[1];
    }

    AtlasNode* AtlasNode::insert(int width, int height)
    {
        AtlasNode* new_node = 0;
        if (_child[0] || _child[1])
        {
            new_node = _child[0]->insert(width, height);
            if (new_node)
                return new_node;

            new_node = _child[1]->insert(width, height);
            if (new_node)
                return new_node;

            return 0;
        }

        if (_id)
            return 0;

        if (width > _rc.getWidth())
            return 0;

        if (height > _rc.getHeight())
            return 0;

        if (width == _rc.getWidth() && height == _rc.getHeight())
            return this;

        _child[0] = new AtlasNode(this, _rc);
        _child[1] = new AtlasNode(this, _rc);

        int dw = _rc.getWidth() - width;
        int dh = _rc.getHeight() - height;

        _child[0]->_rc = _rc;
        _child[1]->_rc = _rc;

        if (dw > dh)
        {
            _child[0]->_rc.setWidth(width);

            _child[1]->_rc.setX(_rc.getX() + width);
            _child[1]->_rc.setWidth(_rc.getWidth() - width);
        }
        else
        {
            _child[0]->_rc.setHeight(height);

            _child[1]->_rc.setY(_rc.getY() + height);
            _child[1]->_rc.setHeight(_rc.getHeight() - height);

        }

        return _child[0]->insert(width, height);
    }


    Atlas::Atlas(): _tree(0), _bounds(0, 0, 0, 0)
    {

    }

    Atlas::~Atlas()
    {
        clean();
    }

    void Atlas::clean()
    {
        delete _tree;
        _tree = 0;
    }

    void Atlas::init(int w, int h)
    {
        delete _tree;
        _tree = new AtlasNode(0, Rect(0, 0, w, h));
    }

    bool Atlas::add(Texture* dest, const ImageData& src, Rect& srcRect)
    {
        AnimationFrame af;
        Point offset(2, 2);
        if (src.w == dest->getWidth())
            offset.x = 0;
        if (src.h == dest->getHeight())
            offset.y = 0;

        int w = src.w + offset.x;
        int h = src.h + offset.y;

        AtlasNode* node = _tree->insert(w, h);
        if (node)
        {
            node->setID(1);
            srcRect = Rect(node->getRect());

            srcRect.size = srcRect.size - offset;
            dest->updateRegion(srcRect.pos.x, srcRect.pos.y, src);

            _bounds.unite(srcRect);

            return true;
        }

        return false;
    }


    Atlas2::Atlas2() : _bounds(0, 0, 0, 0), _skipSize(3)
    {

    }

    void Atlas2::clean()
    {

    }

    void Atlas2::init(int w, int h, int skipSize)
    {
        _skipSize = skipSize;
        _bounds = Rect(0, 0, 0, 0);
        _free.clear();
        //_free.reserve(15);
        _free.push_back(Rect(0, 0, w, h));
    }

    bool sortRects(const Rect& a, const Rect& b)
    {
        return std::min(a.size.x, a.size.y) < std::min(b.size.x, b.size.y);
        //return a.pos.y < b.pos.y;
        //return a.size.x * a.size.y < b.size.x * b.size.y;
        //return a.size.x < b.size.x;
        //return a.size.y < b.size.y;
    }

    bool Atlas2::add(Texture* dest, const ImageData& src, Rect& srcRect)
    {
        Point offset(2, 2);
        if (src.w == dest->getWidth())
            offset.x = 0;
        if (src.h == dest->getHeight())
            offset.y = 0;

        int w = src.w + offset.x;
        int h = src.h + offset.y;

        Point size(w, h);

        for (size_t i = 0; i != _free.size(); ++i)
        {
            const Rect& rect_ = _free[i];
            if (rect_.getWidth() >= w && rect_.getHeight() >= h)
            {
                Rect rect = rect_;

                srcRect.pos = rect.pos;
                srcRect.size = Point(src.w, src.h);

                _bounds.unite(srcRect);

                dest->updateRegion(rect.pos.x, rect.pos.y, src);

                _free.erase(_free.begin() + i);

                Point ds = rect.size - size;


                Rect a = rect;
                Rect b = rect;

                if (ds.x > ds.y)
                    a.setWidth(w);
                else
                    b.setHeight(h);

                b.setWidth(ds.x);
                a.setHeight(ds.y);

                if (!a.isEmpty() && std::min(a.getWidth(), a.getHeight()) > _skipSize)
                {
                    a.pos.y = rect.pos.y + h;
                    rects::iterator i = std::lower_bound(_free.begin(), _free.end(), a, sortRects);
                    _free.insert(i, a);
                }


                if (!b.isEmpty() && std::min(b.getWidth(), b.getHeight()) > _skipSize)
                {
                    b.pos.x = rect.pos.x + w;
                    rects::iterator i = std::lower_bound(_free.begin(), _free.end(), b, sortRects);
                    _free.insert(i, b);
                }

                return true;
            }
        }
        return false;
        ///Rect a = re
    }

    MultiAtlas::MultiAtlas(createTextureCallback cb) : _bounds(0, 0, 0, 0), _skipSize(3), _cb(cb)
    {

    }

    void MultiAtlas::clean()
    {
        _free.clear();
    }

    void MultiAtlas::init(int skipSize)
    {
        _skipSize = skipSize;

        /*
        _bounds = Rect(0, 0, 0, 0);
        _free.clear();
        //_free.reserve(15);
        _free.push_back(Rect(0, 0, w, h));
        */
    }

    bool MultiAtlas::sortRects(const rect& a, const rect& b)
    {
        return std::min(a.rct.size.x, a.rct.size.y) < std::min(b.rct.size.x, b.rct.size.y);
        //return a.pos.y < b.pos.y;
        //return a.size.x * a.size.y < b.size.x * b.size.y;
        //return a.size.x < b.size.x;
        //return a.size.y < b.size.y;
    }

    void MultiAtlas::place(const rect& rct, int w, int h, const ImageData& src, spTexture& t, Rect& srcRect)
    {
        const Rect& rc = rct.rct;
        srcRect.pos = rc.pos;
        srcRect.size = Point(src.w, src.h);
        t = rct.texture;


        Point size(w, h);

        Point ds = rc.size - size;
        rct.texture->updateRegion(rc.pos.x, rc.pos.y, src);


        rect a = rct;
        rect b = rct;

        if (ds.x > ds.y)
            a.rct.setWidth(w);
        else
            b.rct.setHeight(h);

        b.rct.setWidth(ds.x);
        a.rct.setHeight(ds.y);

        if (!a.rct.isEmpty() && std::min(a.rct.getWidth(), a.rct.getHeight()) > _skipSize)
        {
            a.rct.pos.y = rct.rct.pos.y + h;
            rects::iterator i = std::lower_bound(_free.begin(), _free.end(), a, sortRects);
            _free.insert(i, a);
        }


        if (!b.rct.isEmpty() && std::min(b.rct.getWidth(), b.rct.getHeight()) > _skipSize)
        {
            b.rct.pos.x = rct.rct.pos.x + w;
            rects::iterator i = std::lower_bound(_free.begin(), _free.end(), b, sortRects);
            _free.insert(i, b);
        }
    }

    bool MultiAtlas::add(const ImageData& src, Rect& srcRect, spTexture& t)
    {
        Point offset(2, 2);

        int w = src.w + offset.x;
        int h = src.h + offset.y;

        Point size(w, h);

        for (size_t i = 0; i != _free.size(); ++i)
        {
            const rect& rect_ = _free[i];
            if (rect_.rct.getWidth() >= w && rect_.rct.getHeight() >= h)
            {
                rect rct = rect_;

                _bounds.unite(srcRect);

                _free.erase(_free.begin() + i);
                place(rct, w, h, src, t, srcRect);

                return true;
            }
        }

        rect rc;
        rc.texture = _cb(w, h);
        if (!rc.texture)
            return false;

        rc.rct = Rect(0, 0, rc.texture->getWidth(), rc.texture->getHeight());


        place(rc, w, h, src, t, srcRect);


        return true;
        ///Rect a = re
    }
}