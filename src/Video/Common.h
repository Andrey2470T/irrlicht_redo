#pragma once

#include <SDL_video.h>
#include <SDL_opengl.h>

#include "Typedefs.h"

#define TEST_GL_ERROR() testGLError(__FILE__, __LINE__)

//! Defines for snprintf_irr because snprintf method does not match the ISO C
//! standard on Windows platforms.
//! We want int snprintf_irr(char *str, size_t size, const char *format, ...);
#if defined(_MSC_VER)
#define snprintf_irr sprintf_s
#else
#define snprintf_irr snprintf
#endif // _MSC_VER

namespace render
{

enum class OpenGLSpec : u8
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

bool testGLError(const char *file, int line);

}