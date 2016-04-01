#include <sstream>
#include "Sprite.h"
#include "res/ResAnim.h"
#include "RenderState.h"
#include "utils/stringUtils.h"
#include "Stage.h"
#include "Serialize.h"
#include "Material.h"

namespace oxygine
{
    Sprite::Sprite()
    {

    }

    Sprite::~Sprite()
    {
        if (_flags & flag_manageResAnim)
        {
            ResAnim* rs = _frame.getResAnim();
            if (rs)
                rs->getAtlas()->unload();
        }
    }

    void Sprite::copyFrom(const Sprite& src, cloneOptions opt)
    {
        _VStyleActor::copyFrom(src, opt);

        _frame = src._frame;
        animFrameChanged(_frame);

        _vstyle = src._vstyle;
        if (getManageResAnim())
        {
            ResAnim* rs = _frame.getResAnim();
            if (rs)
                rs->getAtlas()->load();
        }
    }

    void Sprite::setManageResAnim(bool manage)
    {
        _flags &= ~flag_manageResAnim;
        if (manage)
            _flags |= flag_manageResAnim;
    }

    std::string Sprite::dump(const dumpOptions& options) const
    {
        std::stringstream stream;
        stream << "{Sprite}\n";
        stream << _vstyle.dump() << " ";
        std::string tname = "null";
        if (_frame.getDiffuse().base)
            tname = _frame.getDiffuse().base->getName();
        stream << "texture='" << tname << "' ";
        //if (_frame.getResAnim())
        //    stream << "resanim='" << _frame.getResAnim()->getName() << "' ";
        if (_flags & flag_manageResAnim)
            stream << "manageResAnim=true";

        stream << Actor::dump(options);
        return stream.str();
    }

    void Sprite::setAnimFrame(const AnimationFrame& f)
    {
        changeAnimFrame(f);
    }


    extern int HIT_TEST_DOWNSCALE;

    bool Sprite::isOn(const Vector2& localPosition)
    {
        if (!Actor::isOn(localPosition))
            return false;

        if (_extendedIsOn)
            return true;

        const HitTestData& ad = _frame.getHitTestData();
        if (!ad.data)
            return true;

        const int BITS = (sizeof(int32_t) * 8);

        const unsigned char* buff = ad.data;
        Vector2 pos = localPosition * _frame.getResAnim()->getAppliedScale();
        Point lp = pos.cast<Point>() / HIT_TEST_DOWNSCALE;
        Rect r(0, 0, ad.w, ad.h);
        if (r.pointIn(lp))
        {
            const int32_t* ints = reinterpret_cast<const int32_t*>(buff + lp.y * ad.pitch);

            int n = lp.x / BITS;
            int b = lp.x % BITS;

            return (ints[n] >> b) & 1;
        }
        return false;
    }

    void Sprite::setFlippedX(bool flippedX)
    {
        if (flippedX != isFlippedX())
        {
            _frame.flipX();
            _flags ^= flag_flipX;
            animFrameChanged(_frame);
        }
    }

    void Sprite::setFlippedY(bool flippedY)
    {
        if (flippedY != isFlippedY())
        {
            _frame.flipY();
            _flags ^= flag_flipY;
            animFrameChanged(_frame);
        }
    }

    void Sprite::setFlipped(bool flippedX, bool flippedY)
    {
        bool fx = flippedX != isFlippedX();
        bool fy = flippedY != isFlippedY();

        if (fx)
        {
            _frame.flipX();
            _flags ^= flag_flipX;
        }

        if (fy)
        {
            _frame.flipY();
            _flags ^= flag_flipY;
        }

        if (fx || fy)
            animFrameChanged(_frame);
    }

    void Sprite::setColumn(int column)
    {
        const ResAnim* rs = getResAnim();
        setAnimFrame(rs, column, getRow());
    }

    void Sprite::setRow(int row)
    {
        const ResAnim* rs = getResAnim();
        setAnimFrame(rs, getColumn(), row);

    }

    void Sprite::setColumnRow(int column, int row)
    {
        const ResAnim* rs = getResAnim();
        setAnimFrame(rs, column, row);
    }

    void Sprite::setResAnim(const ResAnim* resanim, int col, int row)
    {
        setAnimFrame(resanim, col, row);
    }

    void Sprite::setAnimFrame(const ResAnim* resanim, int col, int row)
    {
        //OX_ASSERT(resanim);
        if (!resanim)
        {
            changeAnimFrame(AnimationFrame());
            return;
        }

        if (resanim->getTotalFrames())
        {
            const AnimationFrame& frame = resanim->getFrame(col, row);
            changeAnimFrame(frame);
        }
        else
        {
            AnimationFrame frame;
            frame.setSize(getSize());
            changeAnimFrame(frame);
        }
    }

    void Sprite::changeAnimFrame(const AnimationFrame& frame)
    {
        if (_flags & flag_manageResAnim)
        {
            ResAnim* rs = _frame.getResAnim();
            if (rs)
                rs->getAtlas()->unload();

            rs = frame.getResAnim();
            if (rs)
                rs->getAtlas()->load();
        }

        bool flipX = (_flags & flag_flipX) != 0;
        bool flipY = (_flags & flag_flipY) != 0;
        if (flipX || flipY)
            _frame = frame.getFlipped(flipY, flipX);
        else
            _frame = frame;
        setSize(_frame.getSize());

        animFrameChanged(_frame);
    }

    void Sprite::animFrameChanged(const AnimationFrame& f)
    {

    }

    RectF Sprite::getDestRect() const
    {
        if (_frame.getDiffuse().base)
            return calcDestRectF(_frame.getDestRect(), _frame.getSize());

        return Actor::getDestRect();
    }


    void Sprite::doRender(const RenderState& rs)
    {
        rs.material->doRender(this, rs);
    }

    void Sprite::serialize(serializedata* data)
    {
        _VStyleActor::serialize(data);

        pugi::xml_node node = data->node;

        const ResAnim* rs = getResAnim();
        if (rs)
        {
            node.remove_attribute("size");

            Resource* r = rs->getParent();
            const char* hint = "";
            if (r)
            {
                r = r->getParent();
                if (r)
                    hint = r->getName().c_str();
            }

            if (rs->getName().find(':') == std::string::npos)
            {
                char name[255];
                safe_sprintf(name, "%s:%s", hint, rs->getName().c_str());
                node.append_attribute("resanim").set_value(name);
            }
            else
            {
                node.append_attribute("resanim").set_value(rs->getName().c_str());
            }

            if (_frame.getColumn() != 0)
                node.append_attribute("column").set_value(_frame.getColumn());
            if (_frame.getRow() != 0)
                node.append_attribute("row").set_value(_frame.getRow());
        }

        setAttr(node, "flipX", isFlippedX(), false);
        setAttr(node, "flipY", isFlippedY(), false);

        node.set_name("Sprite");
    }

    void Sprite::deserialize(const deserializedata* data)
    {
        _VStyleActor::deserialize(data);

        pugi::xml_node node = data->node;
        const char* res = node.attribute("resanim").as_string(0);
        if (res && *res)
        {
            int col = node.attribute("column").as_int(0);
            int row = node.attribute("row").as_int(0);
            AnimationFrame frame = data->factory->getFrame(res, col, row);
            setAnimFrame(frame);
        }

        setFlipped(node.attribute("flipX").as_bool(false), node.attribute("flipY").as_bool(false));
    }
}
