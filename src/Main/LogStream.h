#pragma once

#include "BasicIncludes.h"
#include <SDL_log.h>
#include <sstream>

namespace main
{

enum LogType
{
	LT_WARNING,
	LT_INFO,
	LT_ERROR
};

// Simple class wrapper over the SDL_Log* functions for simpler and shorter calls
class LogStream
{
	const LogType Type;

	std::ostringstream Stream;

public:
	LogStream(LogType type)
		: Type(type)
	{}

	LogStream(const LogStream &other) = delete; // prohibit the copying

    LogStream &operator<<(const std::string &str);

    LogStream &operator<<(s32 num)
	{
		return *this << std::to_string(num);
	}

    LogStream &operator<<(u32 num)
	{
		return *this << std::to_string(num);
	}

    LogStream &operator<<(f32 num)
	{
		return *this << std::to_string(num);
	}

private:
	void log(const std::string &str);
};

// Standard log streams
extern LogStream WarnStream;
extern LogStream InfoStream;
extern LogStream ErrorStream;

}
