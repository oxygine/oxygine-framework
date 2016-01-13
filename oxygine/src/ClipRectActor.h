#pragma once
#include "oxygine_include.h"
#include "Actor.h"

namespace oxygine
{
    DECLARE_SMART(ClipRectActor, spClipRectActor);
    /**
    ClipRectActor clips all out of bound children. Rotation is not supported
    */
    class ClipRectActor : public _Actor
    {
    public:
        DECLARE_COPYCLONE_NEW(ClipRectActor);
        ClipRectActor();
        ~ClipRectActor();

        bool getClipping() const { return _clipping; }

        /**Enables clipping for children. By default is on*/
        void setClipping(bool enable) {_clipping = enable;}

        void render(const RenderState& rs);


        void handleEvent(Event* event);

        void serialize(serializedata* data);
        void deserialize(const deserializedata* data);

    protected:

        bool _clipping;
    };
}

#ifdef OX_EDITOR
#include "EditorClipRectActor.h"
#else
typedef oxygine::ClipRectActor BaseClipRectActor;
#endif
