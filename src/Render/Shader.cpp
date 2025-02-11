#include "Shader.h"
#include <fstream>
#include <sstream>
#include "UniformBuffer.h"

namespace render
{

std::string readFile(const std::string &path)
{
	std::ifstream file;
	file.open(fs::path(path));

	if (!file.is_open()) {
		ErrorStream << "readFile() failed to open the file\n";
		return "";
	}
	std::stringstream strBuffer;
	strBuffer << file.rdbuf();

	return strBuffer.str();
}

Shader::Shader(const fs::path &vsPath, const fs::path &fsPath, const fs::path &gsPath)
{
	if (vsPath == "" || fsPath == "")
	{
        ErrorStream << "Shader:Shader() empty paths to the vertex or fragment shaders\n";
		return;
	}

	vertexShaderID = createShader(GL_VERTEX_SHADER, readFile(vsPath));
	fragmentShaderID = createShader(GL_FRAGMENT_SHADER, readFile(fsPath));
	geometryShaderID = createShader(GL_GEOMETRY_SHADER, readFile(gsPath));

	programID = createProgram();
}

Shader::Shader(const std::string &vsCode, const std::string &fsCode, const std::string &gsCode)
{
    if (vsCode == "" || fsCode == "")
	{
        ErrorStream << "Shader:Shader() empty code strings of the vertex or fragment shaders\n";
		return;
	}

	vertexShaderID = createShader(GL_VERTEX_SHADER, vsCode);
	fragmentShaderID = createShader(GL_FRAGMENT_SHADER, fsCode);
	geometryShaderID = createShader(GL_GEOMETRY_SHADER, gsCode);

	programID = createProgram();
}

Shader::~Shader()
{
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	if (geometryShaderID != 0)
		glDeleteShader(geometryShaderID);

	glDeleteProgram(programID);
}

void Shader::setUniformFloat(const std::string &name, f32 value)
{
	glUniform1f(getUniformLocation(name), value);
}
void Shader::setUniformInt(const std::string &name, s32 value)
{
	glUniform1i(getUniformLocation(name), value);
}
void Shader::setUniformUInt(const std::string &name, u32 value)
{
	glUniform1ui(getUniformLocation(name), value);
}

void Shader::setUniformFloatArray(const std::string &name, std::vector<f32> values)
{
	glUniform1fv(getUniformLocation(name), values.size(), values.data());
}
void Shader::setUniformIntArray(const std::string &name, std::vector<s32> values)
{
	glUniform1iv(getUniformLocation(name), values.size(), values.data());
}
void Shader::setUniformUIntArray(const std::string &name, std::vector<u32> values)
{
	glUniform1uiv(getUniformLocation(name), values.size(), values.data());
}

void Shader::setUniform2Float(const std::string &name, utils::v2f value)
{
	glUniform2f(getUniformLocation(name), value.X, value.Y);
}
void Shader::setUniform2Int(const std::string &name, utils::v2i value)
{
	glUniform2i(getUniformLocation(name), value.X, value.Y);
}
void Shader::setUniform2UInt(const std::string &name, utils::v2u value)
{
	glUniform2ui(getUniformLocation(name), value.X, value.Y);
}

void Shader::setUniform3Float(const std::string &name, utils::v3f value)
{
	glUniform3f(getUniformLocation(name), value.X, value.Y, value.Z);
}
void Shader::setUniform3Int(const std::string &name, utils::v3i value)
{
	glUniform3i(getUniformLocation(name), value.X, value.Y, value.Z);
}
void Shader::setUniform3UInt(const std::string &name, utils::v3u value)
{
	glUniform3ui(getUniformLocation(name), value.X, value.Y, value.Z);
}

void Shader::setUniform4x4Matrix(const std::string &name, utils::matrix4 value)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, value.pointer());
}

void Shader::setUniformBlock(const std::string &name, UniformBuffer *ubo)
{
	u32 block_index = glGetUniformBlockIndex(programID, name.c_str());
	glUniformBlockBinding(programID, block_index, ubo->getBindingPoint());
}


u32 Shader::createShader(GLenum shaderType, const std::string &code)
{
	if (code.empty())
		return 0;

	GLuint shader = glCreateShader(shaderType);
    const char *cstr_code = code.c_str();
    glShaderSource(shader, 1, &cstr_code, nullptr);
	glCompileShader(shader);

    GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar *infoLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, nullptr, infoLog);

        ErrorStream << "Shader::createShader() the shader failed to compile: " << std::string(infoLog) << "\n";
		return 0;
	}

	return shader;
}

u32 Shader::createProgram()
{
	GLuint program = glCreateProgram();
    glAttachShader(program, vertexShaderID);
    glAttachShader(program, fragmentShaderID);

	if (geometryShaderID != 0)
        glAttachShader(program, geometryShaderID);

	glLinkProgram(program);

    GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar *infoLog = new GLchar[maxLength];
		glGetProgramInfoLog(program, maxLength, nullptr, infoLog);

		ErrorStream << "Shader::createProgram() the program failed to link: " << std::string(infoLog) << "\n";
		return 0;
	}

	return program;
}

u32 Shader::getUniformLocation(const std::string &name)
{
	auto found = uniforms.find(name);

	if (found != uniforms.end())
        return found->second;
	else {
		u32 location = glGetUniformLocation(programID, name.c_str());
		uniforms[name] = location;
		return location;
	}
}

}
