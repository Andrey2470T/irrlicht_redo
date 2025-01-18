#include "Common.h"
#include <sstream>

namespace render
{

GLParameters::GLParameters()
{
    version = glGetString(GL_VERSION);
	vendor = glGetString(GL_VENDOR);

    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
	glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize);
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndexCount);
	glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &maxTextureLODBias);
}

void GLAPIENTRY debugCB(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	// shader compiler can be very noisy
	if (source == GL_DEBUG_SOURCE_SHADER_COMPILER && severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	SDL_LogPriority priority = SDL_LOG_PRIORITY_INFO;
	if (severity == GL_DEBUG_SEVERITY_HIGH)
		priority = SDL_LOG_PRIORITY_ERROR;
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		priority = SDL_LOG_PRIORITY_WARN;

	std::stringstream debugLog;

	debugLog << (u32)source << " ";
	debugLog << (u32)type << " ";
	debugLog << message;

    SDL_LogMessage(LC_VIDEO, priority, debugLog.str().c_str());
}

void enableErrorTest()
{
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debugCB, nullptr);
}

bool testGLError(const char *file, int line)
{
	std::string file_p(file);

	GLenum g = glGetError();
	std::string err = "";
	switch (g) {
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		err = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		err = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		err = "GL_INVALID_OPERATION";
		break;
	case GL_STACK_OVERFLOW:
		err = "GL_STACK_OVERFLOW";
		break;
	case GL_STACK_UNDERFLOW:
		err = "GL_STACK_UNDERFLOW";
		break;
	case GL_OUT_OF_MEMORY:
		err = "GL_OUT_OF_MEMORY";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		err = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
#ifdef GL_VERSION_4_5
	case GL_CONTEXT_LOST:
		err = "GL_CONTEXT_LOST";
		break;
#endif
	};

	// Empty the error queue, see <https://www.khronos.org/opengl/wiki/OpenGL_Error>
	bool multiple = false;
	while (glGetError() != GL_NO_ERROR)
		multiple = true;

	// basename
	for (char sep : {'/', '\\'}) {
		auto basename_pos = file_p.find_last_of(sep);

		if (basename_pos != file_p.npos)
			file_p = file_p.substr(basename_pos+1);
	}

	std::stringstream debugLog;

	debugLog << err << " ";
	debugLog << file_p << ":";
	debugLog << line;

	if (multiple)
		debugLog << " (older errors exist)";

	SDL_LogError(LC_VIDEO, debugLog.str().c_str());

	return true;
}

}
