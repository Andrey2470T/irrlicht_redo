#include "Common.h"

namespace render
{

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

	std::string log_output = err + " " + file_p + ":" + std::to_string(line);
	log_output += (multiple ? " (older errors exist)" : "");

	SDL_LogError(LC_VIDEO, log_output.c_str());

	return true;
}

}
