#pragma once

#include <GL/glew.h>

#include "ExtBasicIncludes.h"

#include "toGLEnum.h"

#define TEST_GL_ERROR() testGLError(__FILE__, __LINE__)

namespace render
{

void enableErrorTest();
bool testGLError(const char *file, int line);

}
