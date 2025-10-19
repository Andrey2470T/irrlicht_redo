#pragma once

#include "BasicIncludes.h"
#include <chrono>

namespace core
{

using namespace std::chrono;

// Platform-independent controllable time counter.
class TimeCounter
{
	u32 LastFixedVirtualTime;  // retrieved from the steady_clock
	u32 OwnTime;    // starting from 0 always
	bool IsStopped;
public:
	TimeCounter(bool do_start = false)
		: LastFixedVirtualTime(0), OwnTime(0), IsStopped(true)
	{
		if (do_start)
			start();
	}

	void start()
	{
		if (!IsStopped)
			return;

		IsStopped = false;

        LastFixedVirtualTime = getRealTime();
	}

	void tick()
	{
        u32 curVirtualTime = getRealTime();

		OwnTime += (curVirtualTime - LastFixedVirtualTime);

		LastFixedVirtualTime = curVirtualTime;
	}

	void reset()
	{
		OwnTime = 0;
	}

	u32 getTimerTime() const
	{
		return OwnTime;
	}

	void stop()
	{
		if (IsStopped)
			return;

		IsStopped = true;

		tick();
	}

	bool isStopped() const
	{
		return IsStopped;
	}

	u32 getElapsed() const
	{
		if (IsStopped)
			return 0;

        u32 curVirtualTime = getRealTime();

		return curVirtualTime - LastFixedVirtualTime;
	}

    static u32 getRealTime()
	{
        return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
	}
};

}
