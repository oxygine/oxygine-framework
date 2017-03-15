#include "AtlasBuilder.h"
#include "core/Texture.h"
#include "Image.h"
#include "core/ImageDataOperations.h"

namespace oxygine
{
    AtlasBuilder::AtlasBuilder() : _bounds(0, 0, 0, 0), _skipSize(3)
    {

    }

    void AtlasBuilder::clean()
    {
        _free.clear();
    }

    void AtlasBuilder::init(int w, int h, int skipSize)
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

    bool AtlasBuilder::add(Texture* dest, const ImageData& src, Rect& srcRect, const Point& offset_)
    {
        Point offset = offset_;
        if (dest)
        {
            if (src.w == dest->getWidth())
                offset.x = 0;
            if (src.h == dest->getHeight())
                offset.y = 0;
        }

        int w = src.w + offset.x * 2;
        int h = src.h + offset.y * 2;

        Point size(w, h);

        for (size_t i = 0; i != _free.size(); ++i)
        {
            const Rect& rect_ = _free[i];
            if (rect_.getWidth() >= w && rect_.getHeight() >= h)
            {
                Rect rect = rect_;

                srcRect.pos = rect.pos + offset;
                srcRect.size = Point(src.w, src.h);

                if (_bounds.isEmpty())
                    _bounds = srcRect;
                else
                    _bounds.unite(srcRect);

                if (dest)
                    dest->updateRegion(srcRect.pos.x, srcRect.pos.y, src);

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
