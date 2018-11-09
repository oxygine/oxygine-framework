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

        timeMS  getTime() const;//deprecated, use F 
        int     getFixedStep() const;//deprecated, use F
        void    setFixedStep(float stepMS);//deprecated, use F
        timeMS  doTick();//deprecated, use F
        timeMS  getLastUpdateTime() const;//deprecated, use F
        int     getLastDT() const;//deprecated, use F

        /**returns current clock time in seconds*/
        float   getTimeF() const;
        int     getPauseCounter() const;
        
        float   getFixedStepF() const;
        float   getLastDTF() const;
        
        float   getLastUpdateTimeF() const;
        float   getMultiplier() const;

        
        /**set fixed actor update in seconds*/
        void    setFixedStepF(float step);
        void    setMultiplier(float m);

        void    pause();
        void    resume();
        void    resetPause();

        void    update(timeMS globalTime = -1);
        
        float   doTickF();

        std::string dump() const;

    private:
        int     _counter;
        double  _destTime;
        double  _srcTime;

        float   _multiplier;
        float   _fixedStep;

        double _lastDT;
        double _lastUpdateTime;
    };
}