// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// 2017 modified by Michael Zeilfelder (unifying extension handlers)
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "ExtensionHandler.h"

#include "irrString.h"
#include "SMaterial.h"
#include "fast_atof.h"
#include "Logger.h"
#include <sstream>


namespace video
{

void ExtensionHandler::initExtensions()
{
	/* OpenGL 3 & ES 3 way to enumerate extensions */
	GLint ext_count = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);
	// clear error which is raised if unsupported
	while (glGetError() != GL_NO_ERROR) {}
		extensions.reserve(ext_count);
	for (GLint k = 0; k < ext_count; k++) {
		auto tmp = glGetStringi(GL_EXTENSIONS, k);
		if (tmp)
			extensions.emplace((char*)tmp);
	}
	if (!extensions.empty())
		return;

	/* OpenGL 2 / ES 2 way to enumerate extensions */
	auto ext_str = glGetString(GL_EXTENSIONS);
	if (!ext_str)
		return;
	// get the extension string, chop it up
	std::istringstream ext_ss((char*)ext_str);
	std::string tmp;
	while (std::getline(ext_ss, tmp, ' '))
		extensions.emplace(tmp);
}

} // end namespace video
