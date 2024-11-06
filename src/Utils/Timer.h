#pragma once

#include <chrono>

namespace utils
{

using time_point = std::chrono::time_point<std::chrono::system_clock>;

class Timer
{
	time_point startTime;
	time_point lastTime;
	bool isStopped;
public:
	Timer() : startTime(0), lastTime(0), isStopped(true)
	{}
	
	void start()
	{
		if (!isStopped)
			return;
		
		isStopped = false;
		
		startTime = std::chrono::system_clock::now();
		lastTime = startTime;
	}
	
	void stop()
	{
		if (isStopped)
			return;
		
		isStopped = true;
		
		lastTime = std::chrono::system_clock::now();
	}
	
	bool isStopped() const
	{
		return isStopped;
	}
	
	u32 getElapsed() const
	{
		u32 endTime;
		
		if (isStopped)
			endTime = lastTime;
		else
			endTime = std::chrono::system_clock::now();
		
		return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
	}
	
	static u32 getRealTime() const
	{
		return std::chrono::system_clock::now();
	}
};

}
