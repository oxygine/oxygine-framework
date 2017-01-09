#pragma once
#include "oxygine-include.h"

namespace oxygine
{
    class UpdateState
    {
    public:
        UpdateState(): time(0), dt(0), iteration(0) {}

        /**local time*/
        timeMS time;

        /**delta time since last update*/
        timeMS dt;

        /**current iteration, used with fixed Clock update */
        int iteration;
    };
}