#pragma once

#include <SDL_video.h>
#include <GL/glew.h>

#include "BasicIncludes.h"

#include "toGLEnum.h"

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

struct GLParameters
{
    const u8 *version;
    const u8 *vendor;

    s32 maxTextureUnits;
    s32 maxColorAttachments;

    s32 maxTextureSize;
    s32 maxCubeMapTextureSize;

    s32 maxIndexCount;
	f32 maxTextureLODBias;

	GLParameters();
};

void enableErrorTest();
bool testGLError(const char *file, int line);

}
