#pragma once
#include "oxygine-include.h"

namespace oxygine
{
    struct serializedata;
    struct deserializedata;
    struct deserializeLinkData;

    class Serializable
    {
    public:
        virtual void serialize(serializedata*) {}
        virtual void deserialize(const deserializedata*) {}
        /**link phase, called when all actors already created*/
        virtual void deserializeLink(const deserializeLinkData*) {}
    };
}