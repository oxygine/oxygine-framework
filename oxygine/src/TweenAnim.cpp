#include "TweenAnim.h"
#include "Sprite.h"
#include "res/ResAnim.h"

namespace oxygine
{
    TweenAnim::TweenAnim(const ResAnim* resAnim, int row):
        _resAnim(resAnim),
        _row(row),
        _start(0),
        _end(0)
    {
        if (_resAnim)
            _end = _resAnim->getColumns();
    }

    TweenAnim::TweenAnim(const ResAnim* resAnim, int startFrame, int endFrame):
        _resAnim(resAnim),
        _row(-1),
        _start(0),
        _end(0)
    {

    }

    void TweenAnim::init(Sprite& actor)
    {

    }


    void TweenAnim::setResAnim(const ResAnim* resAnim)
    {
        _resAnim = resAnim;
        if (_resAnim && _row != -1)
            _end = _resAnim->getColumns();
    }


    void TweenAnim::setColumns(int start, int end)
    {
        _start = start;
        _end = end;
    }

    void TweenAnim::setInterval(int start, int end)
    {
        _start = start;
        _end = end;
    }


    void TweenAnim::update(Sprite& actor, float p, const UpdateState& us)
    {
        OX_ASSERT(_resAnim);
        int frame = interpolate<int>(_start, _end, p);

        if (_start > _end)
        {
            if (frame <= _end)
                frame = _end + 1;
            OX_ASSERT(frame <= _start && frame > _end);
        }
        else
        {
            if (frame >= _end)
                frame = _end - 1;
            OX_ASSERT(frame >= _start && frame < _end);
        }

        if (_row == -1)
            actor.setAnimFrame(_resAnim->getFrame(frame));
        else
            actor.setAnimFrame(_resAnim->getFrame(frame, _row));
    }
}