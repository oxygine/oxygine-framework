#include <sstream>
#include "Sprite.h"
#include "res/ResAnim.h"
#include "core/Renderer.h"
#include "RenderState.h"
#include "utils/stringUtils.h"
#include "Stage.h"
#include "Serialize.h"

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
        if (_flags & flag_manageResAnim)
            stream << "manageResAnim=true";

        stream << Actor::dump(options);
        return stream.str();
    }

    void Sprite::setAnimFrame(const AnimationFrame& f)
    {
        changeAnimFrame(f);
    }

    void Sprite::setRow(int row, int column)
    {
        const ResAnim* rs = getResAnim();
        if (column == -1)
            column = getColumn();
        setAnimFrame(rs, column, row);

    }

    void Sprite::setColumn(int column, int row)
    {
        const ResAnim* rs = getResAnim();
        if (row == -1)
            row = getRow();
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

        _frame = frame;
        setSize(_frame.getFrameSize());

        animFrameChanged(_frame);
    }

    void Sprite::animFrameChanged(const AnimationFrame& f)
    {

    }

    RectF Sprite::getDestRect() const
    {
        if (_frame.getDiffuse().base)
            return calcDestRectF(_frame.getDestRect(), _frame.getFrameSize());

        return Actor::getDestRect();
    }


    void Sprite::doRender(const RenderState& rs)
    {
        /*
        _vstyle._apply(rs);
        const Diffuse &df = _frame.getDiffuse();
        #ifdef EMSCRIPTEN
        if (df.base && df.base->getHandle())
        #else
        if (df.base)
        #endif
        {
            rs.renderer->setDiffuse(df);

            RectF destRect = getDestRect();
            rs.renderer->draw(_frame.getSrcRect(), destRect);
        }

        */

        _vstyle._apply(rs);

        const Diffuse& df = _frame.getDiffuse();
        const spNativeTexture& base = df.base;
#ifdef EMSCRIPTEN
        if (base && base->getHandle())
#else
        if (base)
#endif
        {
            rs.renderer->setTexture(df.base, df.alpha, df.premultiplied);
            rs.renderer->draw(&rs, getColor(), _frame.getSrcRect(), getDestRect());
        }
    }

    void Sprite::serialize(serializedata* data)
    {
        _VStyleActor::serialize(data);

        pugi::xml_node node = data->node;
        node.remove_attribute("size");

        const ResAnim* rs = getResAnim();
        if (rs)
        {
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

        node.set_name("Sprite");
    }

    void Sprite::deserialize(const deserializedata* data)
    {
        _VStyleActor::deserialize(data);

        pugi::xml_node node = data->node;
        const char* res = node.attribute("resanim").as_string(0);
        if (res)
        {
            ResAnim* rs = safeCast<ResAnim*>(data->factory->getResAnim(res));

            setResAnim(rs);
        }
    }
}
