#pragma once

#include <SDL_video.h>
#include <GL/glew.h>

#include "ExtBasicIncludes.h"

#include "toGLEnum.h"

#define TEST_GL_ERROR() testGLError(__FILE__, __LINE__)

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
