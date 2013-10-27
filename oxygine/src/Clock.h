#pragma once
#include "oxygine_include.h"
#include "core/Object.h"

namespace oxygine
{	
	class UpdateState;



	DECLARE_SMART(Clock, spClock);
	class Clock: public Object
	{
	public:
		Clock();
		~Clock();

		timeMS	getTime() const;
		int		getPauseCounter() const;
		int		getFixedStep() const;
		float	getMultiplier() const;

		void	setFixedStep(float stepMS);
		void	setMultiplier(float m);		

		void	pause();
		void	resume();

		void	update(timeMS globalTime = -1);
		timeMS  doTick();

		string dump() const;
	private:
		int		_counter;
		double	_destTime;
		double	_srcTime;
		
		float	_multiplier;
		float	_fixedStep;

		timeMS _lastUpdateTime;
	};
}