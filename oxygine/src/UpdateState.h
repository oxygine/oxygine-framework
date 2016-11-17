#pragma once
#include "oxygine-include.h"
namespace oxygine
{
    class UpdateState
    {
    public:
        UpdateState(): time(0), dt(0), iteration(0) {}

        timeMS time;
        timeMS dt;
        int iteration;
    };
}