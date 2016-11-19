#pragma once
#include "oxygine-include.h"

namespace oxygine
{
    class Serializable
    {
    public:
        virtual void serialize(serializedata*) {}
        virtual void deserialize(const deserializedata*) {}
        /**link phase, called when all actors already created*/
        virtual void deserializeLink(const deserializeLinkData*) {}
    };
}