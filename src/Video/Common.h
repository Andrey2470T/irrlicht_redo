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

class COpenGLCoreFeature;

template <class TOpenGLDriver>
class COpenGLCoreTexture;

class COpenGL3DriverBase;
typedef COpenGLCoreTexture<COpenGL3DriverBase> COpenGL3Texture;

class OpenGLVBO;

enum OpenGLSpec : u8
{
	Core,
	Compat,
	ES,
	// WebGL, // TODO
};

struct OpenGLVersion
{
	OpenGLSpec Spec;
	u8 Major;
	u8 Minor;
	u8 Release;
};

}
