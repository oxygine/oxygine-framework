#include "Clock.h"
#include <sstream>

#ifdef __S3E__
#   include "s3eTimer.h"
#elif _WIN32
#   include <windows.h>
#elif __ANDROID__
#   include "core/android/jniUtils.h"
#elif EMSCRIPTEN
#include <sys/time.h>
#include "SDL_timer.h"
#elif __APPLE__
#   include <CoreFoundation/CFDate.h>
#   include <sys/time.h>
#endif

#if OXYGINE_SDL
#include "SDL.h"
#endif

namespace oxygine
{
    Clock::Clock():
        _counter(0), _destTime(0), _srcTime(0),
        _multiplier(1.0f), _fixedStep(0),
        _lastUpdateTime(-1)
    {
    }

    Clock::~Clock()
    {

    }

    float Clock::getMultiplier() const
    {
        return _multiplier;
    }

    int Clock::getFixedStep() const
    {
        return (int)_fixedStep;
    }

    void Clock::setMultiplier(float m)
    {
        _multiplier = m;
    }

    void Clock::setFixedStep(float step)
    {
        _fixedStep = step;
    }

    void Clock::pause()
    {
        _counter += 1;
    }

    void Clock::resume()
    {
        _counter -= 1;
        //OX_ASSERT(_counter >= 0);
    }
    void Clock::resetPause()
    {
        _counter = 0;
    }

    void Clock::update(timeMS globalTime)
    {
        timeMS time = globalTime;
        const double neg = -1;
        if (time == neg)
            time = getTimeMS();

        if (_lastUpdateTime == neg)
            _lastUpdateTime = time;

        double dt = (time - _lastUpdateTime) * _multiplier;
        if (dt < 1 && dt > 0)
            dt = 1;

        if (dt > 100)
            dt = 100;
        if (dt < 0)
            dt = 1;

        if (_counter > 0)
            dt = 0;//todo destTime == srcTime ??

        //log::messageln("dt: %x %d", this, dt);
        _destTime += dt;

        _lastUpdateTime = time;

        //if (_fixedStep > 0)
        //  printf("ticks: %d\n", int((_destTime - _srcTime)/_fixedStep));
    }

    timeMS Clock::doTick()
    {
        if (_counter > 0)
            return 0;

        if (_srcTime + _fixedStep > _destTime)
            return 0;

        if (_fixedStep == 0)
        {
            timeMS dt = (timeMS)(_destTime - _srcTime);
            //OX_ASSERT(dt <= 100);
            _srcTime = _destTime;
            return dt;
        }

        _srcTime += _fixedStep;
        return (timeMS)_fixedStep;
    }

    timeMS Clock::getTime() const
    {
        return (timeMS)_srcTime;
    }

    int Clock::getPauseCounter() const
    {
        return _counter;
    }

    std::string Clock::dump() const
    {
        std::stringstream stream;
        stream << "clock={time=" << getTime() << "ms";
        if (_counter)
            stream << "paused=" << _counter;
        if (_multiplier != 1.0f)
            stream << ", multiplier=" << _multiplier;
        if (_fixedStep)
            stream << ", fixed_step=" << _fixedStep;
        stream << "}";
        return stream.str();
    }


    timeMS getTimeMS()
    {
#if __S3E__
        return (timeMS)s3eTimerGetUST();
#elif EMSCRIPTEN
        return SDL_GetTicks();
#elif OXYGINE_SDL
        return SDL_GetTicks();
#else
#pragma error "getTimeMS not implemented"
#endif
        return 0;
    }

    int64   getTimeUTCMS()
    {
#if __S3E__
        return s3eTimerGetUTC();
#elif _WIN32
        FILETIME tm;
        GetSystemTimeAsFileTime(&tm);
        int64 t = tm.dwLowDateTime + (int64(tm.dwHighDateTime) << 32);
        int64 utc = (t - 116444736000000000LL) / 10000;
        return utc;
#elif __ANDROID__
        return jniGetTimeUTCMS();
#elif EMSCRIPTEN
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64 tm =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_usec) / 1000;
        return tm;
#elif __APPLE__
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64 tm =
            (unsigned long long)(tv.tv_sec) * 1000 +
            (unsigned long long)(tv.tv_usec) / 1000;
        return tm;

        /*
        CFAbsoluteTime tm = CFAbsoluteTimeGetCurrent();
        int64 v = int64(tm * 1000.0);
        return v;
         */
#endif
        return getTimeMS();
    }

    int     getTimeUTC()
    {
        return (int)(getTimeUTCMS() / 1000);
    }
}