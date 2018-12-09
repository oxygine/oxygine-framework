#pragma once
#include "oxygine-include.h"

namespace oxygine
{
    class UpdateState
    {
    public:
        UpdateState(): time(0), dt(0), iteration(0), dtf(0) {}

        /**local time in  milliseconds*/
        timeMS time;

        /**local time in  seconds*/
        float timef;

        /**delta time since last update in milliseconds*/
        timeMS dt;

        /*delta time since last update in seconds*/
        float dtf;

        /**current iteration, used with fixed Clock update */
        int iteration;
    };
}