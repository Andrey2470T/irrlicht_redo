#pragma once

#include <chrono>

namespace main
{

// Platform-independent controllable time counter.
class TimeCounter
{
	u32 LastFixedVirtualTime;  // retrieved from the steady_clock
	u32 OwnTime;    // starting from 0 always
	bool IsStopped;
public:
	Stopwatch(bool do_start = false)
		: LastFixedVirtualTime(0), OwnTime(0), IsStopped(true)
	{
		if (do_start)
			start();
	}

	u32 getVirtualTime() const
	{
		return std::chrono::steady_clock::now().time_since_epoch().count();
	}

	void start()
	{
		if (!IsStopped)
			return;

		IsStopped = false;

		LastFixedVirtualTime = getVirtualTime();
	}

	void tick()
	{
		u32 curVirtualTime = getVirtualTime();

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

		u32 curVirtualTime = getVirtualTime();

		return curVirtualTime - LastFixedVirtualTime;
	}

	static u32 getRealTime() const
	{
		return std::chrono::system_clock::now().time_since_epoch().count();
	}
};

}
