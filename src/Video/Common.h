// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "irrTypes.h"

#include <GL/glew.h>

#ifdef _IRR_USE_SDL3_
	#include <SDL3/SDL_opengl.h>
#else
	#include <SDL_video.h>
	#include <SDL_opengl.h>
#endif


namespace video
{

// Forward declarations.

class OpenGLVBO;

}
