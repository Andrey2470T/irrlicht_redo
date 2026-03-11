// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrTypes.h"
#include "irrString.h"
#include "path.h"
#include "ILogger.h"


namespace os
{
class Byteswap
{
public:
	static u16 byteswap(u16 num);
	static s16 byteswap(s16 num);
	static u32 byteswap(u32 num);
	static s32 byteswap(s32 num);
	static u64 byteswap(u64 num);
	static s64 byteswap(s64 num);
	static f32 byteswap(f32 num);
	// prevent accidental swapping of chars
	static inline u8 byteswap(u8 num) { return num; }
	static inline c8 byteswap(c8 num) { return num; }
};

class Printer
{
public:
	// prints out a string to the console out stdout or debug log or whatever
	static void print(const c8 *message, ELOG_LEVEL ll = ELL_INFORMATION);
	static void log(const c8 *message, ELOG_LEVEL ll = ELL_INFORMATION);

	// The string ": " is added between message and hint
	static void log(const c8 *message, const c8 *hint, ELOG_LEVEL ll = ELL_INFORMATION);
	static void log(const c8 *message, const io::path &hint, ELOG_LEVEL ll = ELL_INFORMATION);
	static ILogger *Logger;
};

} // end namespace os
