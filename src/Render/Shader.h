#pragma once

#include "Common.h"
#include "Utils/Matrix4.h"
#include <unordered_map>
#include "FilesystemVersions.h"

namespace render
{

class UniformBuffer;

class Shader
{
	u32 programID;
	u32 vertexShaderID;
	u32 fragmentShaderID;
	u32 geometryShaderID;

	std::unordered_map<std::string, u32> uniforms;
public:
    Shader(const fs::path &vsPath, const fs::path &fsPath, const fs::path &gsPath="");
	Shader(const std::string &vsCode, const std::string &fsCode, const std::string &gsCode="");

	~Shader();

	u32 getProgram() const
	{
		return programID;
	}

	void use() const
	{
		glUseProgram(programID);
	}
	
	void setUniformFloat(const std::string &name, f32 value);
	void setUniformInt(const std::string &name, s32 value);
	void setUniformUInt(const std::string &name, u32 value);

	void setUniformFloatArray(const std::string &name, std::vector<f32> values);
	void setUniformIntArray(const std::string &name, std::vector<s32> values);
	void setUniformUIntArray(const std::string &name, std::vector<u32> values);

    void setUniform2Float(const std::string &name, utils::v2f value);
    void setUniform2Int(const std::string &name, utils::v2i value);
    void setUniform2UInt(const std::string &name, utils::v2u value);

    void setUniform3Float(const std::string &name, utils::v3f value);
    void setUniform3Int(const std::string &name, utils::v3i value);
    void setUniform3UInt(const std::string &name, utils::v3u value);

    void setUniform4x4Matrix(const std::string &name, utils::matrix4 value);

	void setUniformBlock(const std::string &name, UniformBuffer *ubo);
	
	bool operator==(const Shader *other)
	{
		return programID == other->programID;
	}
	
	bool operator!=(const Shader *other)
	{
		return programID != other->programID;
	}
private:
	u32 createShader(GLenum shaderType, const std::string &code);
	u32 createProgram();
	
	u32 getUniformLocation(const std::string &name);
};

}
