#pragma once
#include "oxygine_include.h"

namespace oxygine
{
    class ResAnim;
    class AnimationFrame;
    class Sprite;
    class UpdateState;

    /** A TweenAnim class
    *   use for playing per frame animation
    \code
    spSprite sprite = new Sprite();
    sprite->addTween(TweenAnim(res.getResAnim("anim")), 500, -1);
    \endcode
    */
    class TweenAnim
    {
    public:
        typedef Sprite type;

        /**Row/Column mode. Plays single row*/
        TweenAnim(const ResAnim* resAnim, int row = 0);

        /**Frames mode. Play animation in interval [start, end]*/
        TweenAnim(const ResAnim* resAnim, int startFrame, int endFrame);

        void init(Sprite& actor);

        const ResAnim*  getResAnim() const {return _resAnim;}
        int             getRow() const {return _row;}
        int             getStart() const {return _start;}
        int             getEnd() const {return _end;}

        /**Changes ResAnim*/
        void setResAnim(const ResAnim* resAnim);

        /**Play animation in interval [start, end]*/
        void setInterval(int start, int end);

        void update(Sprite& actor, float p, const UpdateState& us);

    protected:
        virtual void _setAnimFrame(Sprite&, const AnimationFrame&);
        const ResAnim* _resAnim;
        int _row;
        int _start;
        int _end;
    };
}