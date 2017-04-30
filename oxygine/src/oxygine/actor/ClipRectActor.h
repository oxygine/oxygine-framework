#pragma once
#include "oxygine-include.h"
#include "Actor.h"

namespace oxygine
{
    DECLARE_SMART(ClipRectActor, spClipRectActor);
    /**
    ClipRectActor clips all out of bound children. Rotation is not supported
    */
    class ClipRectActor : public Actor
    {
        INHERITED(Actor);
    public:
        DECLARE_COPYCLONE_NEW(ClipRectActor);
        ClipRectActor();
        ~ClipRectActor();

        bool getClipping() const { return _clipping; }

        /**Enables clipping for children. By default is on*/
        void setClipping(bool enable) {_clipping = enable;}

        void render(const RenderState& rs) override;


        void handleEvent(Event* event) override;

        void serialize(serializedata* data) override;
        void deserialize(const deserializedata* data) override;

    protected:

        bool _clipping;
    };
}

EDITOR_INCLUDE(ClipRectActor);