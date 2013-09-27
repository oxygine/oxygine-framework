#include "Clock.h"
#include <sstream>

#ifdef __S3E__
#include "s3eTimer.h"
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

		if (_counter > 0)
			dt = 0;//todo destTime == srcTime ??

		//log::messageln("dt: %x %d", this, dt);
		_destTime += dt;

		_lastUpdateTime = time;

		//if (_fixedStep > 0)
		//	printf("ticks: %d\n", int((_destTime - _srcTime)/_fixedStep));
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

	int	Clock::getPauseCounter() const
	{
		return _counter;
	}

	string Clock::dump() const
	{
		stringstream stream;
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
}