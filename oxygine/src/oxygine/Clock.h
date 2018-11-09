#pragma once
#include "oxygine-include.h"
#include "core/Object.h"

namespace oxygine
{
    DECLARE_SMART(Clock, spClock);
    class Clock: public Object
    {
    public:
        Clock();
        ~Clock();

        timeMS  getTime() const;
        float   getTimeF() const;
        int     getPauseCounter() const;
        int     getFixedStep() const;
        int     getLastDT() const;
        timeMS  getLastUpdateTime() const;
        float   getMultiplier() const;

        void    setFixedStep(float stepMS);
        void    setMultiplier(float m);

        void    pause();
        void    resume();
        void    resetPause();

        void    update(timeMS globalTime = -1);
        timeMS  doTick();
        float   doTickF();

        std::string dump() const;

    private:
        int     _counter;
        double  _destTime;
        double  _srcTime;

        float   _multiplier;
        float   _fixedStep;

        int    _lastDT;
        timeMS _lastUpdateTime;
    };
}