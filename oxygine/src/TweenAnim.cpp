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
            _end = _resAnim->getColumns() - 1;
    }

    TweenAnim::TweenAnim(const ResAnim* resAnim, int startFrame, int endFrame):
        _resAnim(resAnim),
        _row(-1),
        _start(0),
        _end(0)
    {
        setInterval(startFrame, endFrame);
    }

    void TweenAnim::init(Sprite& actor)
    {

    }


    void TweenAnim::setResAnim(const ResAnim* resAnim)
    {
        _resAnim = resAnim;
        if (_resAnim && _row != -1)
            _end = _resAnim->getColumns() - 1;
    }

    void TweenAnim::setInterval(int start, int end)
    {
        _start = start;
        _end = end;

        int num = _row == -1 ? _resAnim->getTotalFrames() : _resAnim->getColumns();
        OX_ASSERT(_start >= 0 && _start < num);
        OX_ASSERT(_end >= 0 && _end < num);
    }


    void TweenAnim::update(Sprite& actor, float p, const UpdateState& us)
    {
        OX_ASSERT(_resAnim);
        int frame = lerp<int>(_start, _end, p);

        if (_row == -1)
            actor.setAnimFrame(_resAnim->getFrame(frame));
        else
            actor.setAnimFrame(_resAnim->getFrame(frame, _row));
    }
}