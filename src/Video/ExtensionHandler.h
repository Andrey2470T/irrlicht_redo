// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include <unordered_set>

#include "irrTypes.h"
#include "Logger.h"

#include "IVideoDriver.h"
#include "Common.h"


namespace video
{

class ExtensionHandler
{
public:
	ExtensionHandler() = default;

	void initExtensions();

	/// Checks whether a named extension is present
	inline bool isExtensionPresent(const std::string &name) const noexcept
	{
		return extensions.count(name) > 0;;
	}

protected:
	std::unordered_set<std::string> extensions;
};

}
