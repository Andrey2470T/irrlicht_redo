#include "Shader.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace render
{

namespace fs = std::filesystem;

std::string readFile(const std::string &path)
{
	std::ifstream file;
	file.open(fs::path(path));

	std::stringstream strBuffer;
	strBuffer << file.rdbuf();

	return strBuffer.str();
}

Shader::Shader(const std::string &vsPath, const std::string &fsPath, const std::string &gsPath)
{
	if (vsPath == "" || fsPath == "")
	{
		SDL_LogError(VC_VIDEO, "Shader:Shader() empty paths to the vertex or fragment shaders");
		return;
	}

	createShader(GL_VERTEX_SHADER, readFile(vsPath));
	createShader(GL_FRAGMENT_SHADER, readFile(fsPath));
	createShader(GL_GEOMETRY_SHADER, readFile(gsPath));

	createProgram();
}

Shader::Shader(const std::string &vsCode, const std::string &fsCode, const std::string &gsCode)
{
	if (vsCode == "" || fSCode == "")
	{
		SDL_LogError(VC_VIDEO, "Shader:Shader() empty code strings of the vertex or fragment shaders");
		return;
	}

	createShader(GL_VERTEX_SHADER, vsCode);
	createShader(GL_FRAGMENT_SHADER, fsCode);
	createShader(GL_GEOMETRY_SHADER, gsCode);

	createProgram();
}

Shader::~Shader()
{
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	if (geometryShaderID != 0)
		glDeleteShader(geometryShaderID);

	glDeleteProgram(programID);
}

}
