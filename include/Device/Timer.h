// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/irrTypes.h"

namespace os {

class Timer
{
public:
	//! returns the current time in milliseconds
	static u32 getTime();

	//! initializes the real timer
	static void init();

	//! sets the current virtual (game) time
	static void setTime(u32 time);

	//! stops the virtual (game) timer
	static void stop();

	//! starts the game timer
	static void start();

	//! sets the speed of the virtual timer
	static void setSpeed(f32 speed = 1.0f);

	//! gets the speed of the virtual timer
	static f32 getSpeed();

	//! returns if the timer currently is stopped
	static bool isStopped();

	//! makes the virtual timer update the time value based on the real time
	static void tick();

	//! returns the current real time in milliseconds
	static u32 getRealTime();

private:
	static void initVirtualTimer();

	static f32 VirtualTimerSpeed;
	static s32 VirtualTimerStopCounter;
	static u32 StartRealTime;
	static u32 LastVirtualTime;
	static u32 StaticTime;
};

} // end namespace os
