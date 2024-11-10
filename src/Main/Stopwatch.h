#pragma once

#include <chrono>

namespace main
{

class Stopwatch
{
	u32 lastFixedVirtualTime;  // retrieved from the steady_clock
	u32 stopwatchTime;    // starting from 0 always
	bool isStopped;
public:
	Stopwatch(bool do_start = false)
		: lastFixedVirtualTime(0), stopwatchTime(0), isStopped(true)
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
		if (!isStopped)
			return;
		
		isStopped = false;

		lastFixedVirtualTime = getVirtualTime();
	}
	
	void tick()
	{
		u32 curVirtualTime = getVirtualTime();
		
		stopwatchTime += (curVirtualTime - lastFixedVirtualTime);
		
		lastFixedVirtualTime = curVirtualTime;
	}
	
	void reset()
	{
		stopwatchTime = 0;
	}
	
	u32 getStopwatchTime() const
	{
		return stopwatchTime;
	}
	
	void stop()
	{
		if (isStopped)
			return;
		
		isStopped = true;
		
		tick();
	}
	
	bool isStopped() const
	{
		return isStopped;
	}
	
	u32 getElapsed() const
	{
		if (isStopped)
			return 0;

		u32 curVirtualTime = getVirtualTime();
		
		return curVirtualTime - lastFixedVirtualTime;
	}
	
	static u32 getRealTime() const
	{
		return std::chrono::system_clock::now().time_since_epoch().count();
	}
};

}
