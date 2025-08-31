#pragma once

#include "BasicIncludes.h"
#include <SDL_log.h>
#include <sstream>

namespace main
{

class IEventReceiver;

enum LogLevel
{
    LL_NONE,
    LL_ERROR,
    LL_WARNING,
    LL_ACTION,
    LL_INFO,
    LL_VERBOSE,
    LL_TRACE,
    LL_MAX
};

// Simple class wrapper over the SDL_Log* functions for simpler and shorter calls
class IrrLogStream
{
    LogLevel Level;

	std::ostringstream Stream;

    IEventReceiver *Receiver = nullptr;

public:
    IrrLogStream(LogLevel level)
        : Level(level)
	{}

    IrrLogStream(const IrrLogStream &other) = delete; // prohibit the copying

    LogLevel getLogLevel() const
    {
        return Level;
    }

    void setLogLevel(LogLevel level)
    {
        Level = level;
    }

    void setEventReceiver(IEventReceiver *receiver);

    IrrLogStream &operator<<(const std::string &str);

    IrrLogStream &operator<<(s32 num)
	{
		return *this << std::to_string(num);
	}

    IrrLogStream &operator<<(u32 num)
	{
		return *this << std::to_string(num);
	}

    IrrLogStream &operator<<(f32 num)
	{
		return *this << std::to_string(num);
	}

    template<class T>
    IrrLogStream &operator<<(Vector2D<T> v)
    {
        return *this << "x = " << v.X << ", y = " << v.Y;
    }

    template<class T>
    IrrLogStream &operator<<(Vector3D<T> v)
    {
        return *this << "x = " << v.X << ", y = " << v.Y << ", z = " << v.Z;
    }

private:
	void log(const std::string &str);
};

// Standard log streams
extern IrrLogStream WarnStream;
extern IrrLogStream InfoStream;
extern IrrLogStream ErrorStream;

}
