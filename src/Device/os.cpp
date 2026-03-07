// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Device/os.h"
#include "irrString.h"
#include "irrMath.h"

#include <chrono>

#ifdef _IRR_USE_SDL3_
	#include <SDL3/SDL_endian.h>
#else
	#include <SDL_endian.h>
#endif

#define bswap_16(X) SDL_Swap16(X)
#define bswap_32(X) SDL_Swap32(X)
#define bswap_64(X) SDL_Swap64(X)


namespace os
{
u16 Byteswap::byteswap(u16 num)
{
	return bswap_16(num);
}
s16 Byteswap::byteswap(s16 num)
{
	return bswap_16(num);
}
u32 Byteswap::byteswap(u32 num)
{
	return bswap_32(num);
}
s32 Byteswap::byteswap(s32 num)
{
	return bswap_32(num);
}
u64 Byteswap::byteswap(u64 num)
{
	return bswap_64(num);
}
s64 Byteswap::byteswap(s64 num)
{
	return bswap_64(num);
}
f32 Byteswap::byteswap(f32 num)
{
	u32 tmp = IR(num);
	tmp = bswap_32(tmp);
	return (FR(tmp));
}
}


namespace os
{

// ------------------------------------------------------
// virtual timer implementation

using namespace std::chrono;

f32 Timer::VirtualTimerSpeed = 1.0f;
s32 Timer::VirtualTimerStopCounter = 0;
u32 Timer::LastVirtualTime = 0;
u32 Timer::StartRealTime = 0;
u32 Timer::StaticTime = 0;

void Timer::initTimer()
{
	initVirtualTimer();
}

u32 Timer::getRealTime()
{
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

//! returns current virtual time
u32 Timer::getTime()
{
	if (isStopped())
		return LastVirtualTime;

	return LastVirtualTime + (u32)((StaticTime - StartRealTime) * VirtualTimerSpeed);
}

//! ticks, advances the virtual timer
void Timer::tick()
{
	StaticTime = getRealTime();
}

//! sets the current virtual time
void Timer::setTime(u32 time)
{
	StaticTime = getRealTime();
	LastVirtualTime = time;
	StartRealTime = StaticTime;
}

//! stops the virtual timer
void Timer::stopTimer()
{
	if (!isStopped()) {
		// stop the virtual timer
		LastVirtualTime = getTime();
	}

	--VirtualTimerStopCounter;
}

//! starts the virtual timer
void Timer::startTimer()
{
	++VirtualTimerStopCounter;

	if (!isStopped()) {
		// restart virtual timer
		setTime(LastVirtualTime);
	}
}

//! sets the speed of the virtual timer
void Timer::setSpeed(f32 speed)
{
	setTime(getTime());

	VirtualTimerSpeed = speed;
	if (VirtualTimerSpeed < 0.0f)
		VirtualTimerSpeed = 0.0f;
}

//! gets the speed of the virtual timer
f32 Timer::getSpeed()
{
	return VirtualTimerSpeed;
}

//! returns if the timer currently is stopped
bool Timer::isStopped()
{
	return VirtualTimerStopCounter < 0;
}

void Timer::initVirtualTimer()
{
	StaticTime = getRealTime();
	StartRealTime = StaticTime;
}

} // end namespace os
