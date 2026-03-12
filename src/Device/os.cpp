// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "os.h"
#include "irrString.h"
#include "irrMath.h"

#if defined(_IRR_WINDOWS_API_)
// ----------------------------------------------------------------
// Windows specific functions
// ----------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


namespace os
{
//! prints a debuginfo string
void Printer::print(const c8 *message, ELOG_LEVEL ll)
{
	core::stringc tmp(message);
	tmp += "\n";
	OutputDebugStringA(tmp.c_str());
	printf("%s", tmp.c_str());
}

} // end namespace os

#elif defined(_IRR_ANDROID_PLATFORM_)

// ----------------------------------------------------------------
// Android version
// ----------------------------------------------------------------

#include <android/log.h>


namespace os
{

//! prints a debuginfo string
void Printer::print(const c8 *message, ELOG_LEVEL ll)
{
	android_LogPriority LogLevel = ANDROID_LOG_UNKNOWN;

	switch (ll) {
	case ELL_DEBUG:
		LogLevel = ANDROID_LOG_DEBUG;
		break;
	case ELL_INFORMATION:
		LogLevel = ANDROID_LOG_INFO;
		break;
	case ELL_WARNING:
		LogLevel = ANDROID_LOG_WARN;
		break;
	case ELL_ERROR:
		LogLevel = ANDROID_LOG_ERROR;
		break;
	default: // ELL_NONE
		LogLevel = ANDROID_LOG_VERBOSE;
		break;
	}

	// Android logcat restricts log-output and cuts the rest of the message away. But we want it all.
	// On my device max-len is 1023 (+ 0 byte). Some websites claim a limit of 4096 so maybe different numbers on different devices.
	constexpr size_t maxLogLen = 1023;
	size_t msgLen = strlen(message);
	size_t start = 0;
	while (msgLen - start > maxLogLen) {
		__android_log_print(LogLevel, "Irrlicht", "%.*s\n",
				static_cast<int>(maxLogLen), &message[start]);
		start += maxLogLen;
	}
	__android_log_print(LogLevel, "Irrlicht", "%s\n", &message[start]);
}

} // end namespace os

#elif defined(_IRR_EMSCRIPTEN_PLATFORM_)

// ----------------------------------------------------------------
// emscripten version
// ----------------------------------------------------------------

#include <emscripten.h>


namespace os
{

//! prints a debuginfo string
void Printer::print(const c8 *message, ELOG_LEVEL ll)
{
	int log_level;
	switch (ll) {
	case ELL_DEBUG:
		log_level = 0;
		break;
	case ELL_INFORMATION:
		log_level = 0;
		break;
	case ELL_WARNING:
		log_level = EM_LOG_WARN;
		break;
	case ELL_ERROR:
		log_level = EM_LOG_ERROR;
		break;
	default: // ELL_NONE
		log_level = 0;
		break;
	}
	emscripten_log(log_level, "%s", message); // Note: not adding \n as emscripten_log seems to do that already.
}

} // end namespace os

#else

// ----------------------------------------------------------------
// linux/ansi version
// ----------------------------------------------------------------

#include <cstdio>


namespace os
{

//! prints a debuginfo string
void Printer::print(const c8 *message, ELOG_LEVEL ll)
{
	printf("%s\n", message);
}

} // end namespace os

#endif // end linux / emscripten / android / windows

namespace os
{
// The platform independent implementation of the printer
ILogger *Printer::Logger = 0;

void Printer::log(const c8 *message, ELOG_LEVEL ll)
{
	if (Logger)
		Logger->log(message, ll);
}

void Printer::log(const c8 *message, const c8 *hint, ELOG_LEVEL ll)
{
	if (Logger)
		Logger->log(message, hint, ll);
}

void Printer::log(const c8 *message, const io::path &hint, ELOG_LEVEL ll)
{
	if (Logger)
		Logger->log(message, hint.c_str(), ll);
}

} // end namespace os
