#pragma once

#include <string>
#include "EventReceiver.h"

namespace main
{

//! Possible log levels.
//! When used has filter ELL_DEBUG means => log everything and ELL_NONE means => log (nearly) nothing.
//! When used to print logging information ELL_DEBUG will have lowest priority while ELL_NONE
//! messages are never filtered and always printed.
enum LOG_LEVEL
{
	//! Used for printing information helpful in debugging
	LL_DEBUG,

	//! Useful information to print. For example hardware info or something started/stopped.
	LL_INFORMATION,

	//! Warnings that something isn't as expected and can cause oddities
	LL_WARNING,

	//! Something did go wrong.
	LL_ERROR,

	//! Logs with ELL_NONE will never be filtered.
	//! And used as filter it will remove all logging except ELL_NONE messages.
	LL_NONE
};

class Logger
{
	LOG_LEVEL Level;
	EventReceiver *Receiver;

	Logger(EventReceiver *receiver)
		: level(LL_INFORMATION), Receiver(receiver)
	{}
	
	LOG_LEVEL getLogLevel() const
	{
		return level;
	}
	
	void setLogLevel(LOG_LEVEL newLevel)
	{
		level = newLevel;
	}
	
	void log(std::string text, LOG_LEVEL level)
	{
		if (level < Level)
			return;
		
		if (Receiver) {
			Event event;
			event.EventType = ET_LOG_TEXT_EVENT;
			event.LogEvent.Text = text;
			event.LogEvent.Level = ll;
			if (Receiver->OnEvent(event))
				return;
		}
		
		std::cout << text << std::endl;
	}
	
	void log(std::string text, std::string hint, LOG_LEVEL level)
	{
		if (level < Level)
			return;
		
		log(text + ": " + hint, level);
	}
};

}
