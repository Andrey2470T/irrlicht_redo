// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IReferenceCounted.h"
#include "irrString.h"

//! Possible log levels.
//! When used has filter ELL_DEBUG means => log everything and ELL_NONE means => log (nearly) nothing.
//! When used to print logging information ELL_DEBUG will have lowest priority while ELL_NONE
//! messages are never filtered and always printed.
enum ELOG_LEVEL
{
	//! Used for printing information helpful in debugging
	ELL_DEBUG,

	//! Useful information to print. For example hardware info or something started/stopped.
	ELL_INFORMATION,

	//! Warnings that something isn't as expected and can cause oddities
	ELL_WARNING,

	//! Something did go wrong.
	ELL_ERROR,

	//! Logs with ELL_NONE will never be filtered.
	//! And used as filter it will remove all logging except ELL_NONE messages.
	ELL_NONE
};

class IEventReceiver;

namespace os {

//! Interface for logging messages, warnings and errors
class Logger : public virtual IReferenceCounted
{
public:
	Logger(IEventReceiver *r);

	//! Returns the current set log level.
	ELOG_LEVEL getLogLevel() const;

	//! Sets a new log level.	void setLogLevel(ELOG_LEVEL ll) override;
	void setLogLevel(ELOG_LEVEL ll);

	//! Prints out a text into the log
	void log(const c8 *text, ELOG_LEVEL ll = ELL_INFORMATION);

	void log(const core::stringc &text, ELOG_LEVEL ll = ELL_INFORMATION)
	{
		log(text.c_str(), ll);
	}

	//! Prints out a text into the log
	void log(const c8 *text, const c8 *hint, ELOG_LEVEL ll = ELL_INFORMATION);

	void log(const core::stringc &text, const core::stringc &hint, ELOG_LEVEL ll = ELL_INFORMATION)
	{
		log(text.c_str(), hint.c_str(), ll);
	}

	//! Sets a new event receiver
	void setReceiver(IEventReceiver *r);

private:
	ELOG_LEVEL LogLevel;
	IEventReceiver *Receiver;
};

} // end namespace os

extern os::Logger *g_irrlogger;
