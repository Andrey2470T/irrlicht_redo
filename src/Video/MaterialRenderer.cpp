// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Video/MaterialRenderer.h"

#include "Enums/EVertexAttributes.h"
#include "Video/IShaderConstantSetCallBack.h"
#include "Video/VideoDriver.h"
#include "Device/Logger.h"
#include "Common.h"
#include "Video/DrawContext.h"
#include "Video/RenderTarget.h"
#include "GLSpecificInfo.h"
#include "Video/Texture.h"


namespace video
{

Shader::Shader(
    const std::string &vertexShaderCode,
    const std::string &fragmentShaderCode,
    const std::string &geometryShaderCode)
{
    VertexShaderID = createShader(EST_VERTEX, vertexShaderCode);
    FragmentShaderID = createShader(EST_FRAGMENT, fragmentShaderCode);

    if (!geometryShaderCode.empty())
        GeometryShaderID = createShader(EST_GEOMETRY, geometryShaderCode);

    createProgram();
}

Shader::~Shader()
{
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    if (GeometryShaderID)
        glDeleteShader(GeometryShaderID);

    glDeleteProgram(ProgramID);
}

std::array<GLenum, 3> toGLShaderType = {
    GL_VERTEX_SHADER,
    GL_GEOMETRY_SHADER,
    GL_FRAGMENT_SHADER
};

u32 Shader::createShader(E_SHADER_TYPE shaderType, const std::string &code)
{
    if (code.empty())
        return 0;

    GLuint shader = glCreateShader(toGLShaderType[shaderType]);
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

        g_irrlogger->log("GLSL shader failed to compile: ", infoLog, ELL_ERROR);

        delete[] infoLog;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void Shader::createProgram()
{
    GLuint program = glCreateProgram();
    glAttachShader(program, VertexShaderID);
    glAttachShader(program, FragmentShaderID);

    if (GeometryShaderID != 0)
        glAttachShader(program, GeometryShaderID);

    for (size_t i = 0; i < EVA_COUNT; ++i)
        glBindAttribLocation(program, i, sBuiltInVertexAttributeNames[i]);

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        GLchar *infoLog = new GLchar[maxLength];
        glGetProgramInfoLog(program, maxLength, nullptr, infoLog);

        g_irrlogger->log("GLSL program failed to link: ", infoLog, ELL_ERROR);

        return;
    }

    ProgramID = program;
}

s32 Shader::getUniformLocation(const std::string &name)
{
    auto found = Uniforms.find(name);

    if (found != Uniforms.end())
        return found->second;
    else {
        s32 location = glGetUniformLocation(ProgramID, name.c_str());

        if (location >= 0)
            Uniforms[name] = location;
        return location;
    }
}

MaterialRenderer::MaterialRenderer(
    VideoDriver *driver,
    s32 &outMaterialTypeNr,
    const std::string &vertexShaderProgram,
    const std::string &fragmentShaderProgram,
    const std::string &debugName,
    IShaderConstantSetCallBack *callback,
    E_MATERIAL_TYPE baseMaterial,
    s32 userData) :
        Driver(driver), CallBack(callback), Alpha(false), Blending(false), UserData(userData)
{
	switch (baseMaterial) {
	case EMT_TRANSPARENT_VERTEX_ALPHA:
	case EMT_TRANSPARENT_ALPHA_CHANNEL:
		Alpha = true;
		break;
	case EMT_ONETEXTURE_BLEND:
		Blending = true;
		break;
	default:
		break;
	}

	if (CallBack)
		CallBack->grab();

    init(outMaterialTypeNr, vertexShaderProgram, fragmentShaderProgram, debugName);
}

MaterialRenderer::~MaterialRenderer()
{
	if (CallBack)
		CallBack->drop();
}

void MaterialRenderer::init(s32 &outMaterialTypeNr,
    const std::string &vertexShaderCode,
    const std::string &fragmentShaderCode,
    const std::string &debugName,
    bool addMaterial)
{
	outMaterialTypeNr = -1;

	ShaderObj = std::make_unique<Shader>(vertexShaderCode, fragmentShaderCode);

    if (!debugName.empty()) {
        Driver->GLInfo->ObjectLabel(GL_PROGRAM, ShaderObj->ProgramID, debugName.c_str());
    }

	if (addMaterial)
        outMaterialTypeNr = Driver->addMaterialRenderer(this);
}

bool MaterialRenderer::OnRender(scene::E_VERTEX_TYPE vtxtype)
{
    if (CallBack)
		CallBack->OnSetUniforms(this, UserData);

	return true;
}

void MaterialRenderer::OnSetMaterial(const video::SMaterial &material,
		const video::SMaterial &lastMaterial,
		bool resetAllRenderstatess)
{
	auto ctxt = Driver->getContext();

    ctxt->setProgram(ShaderObj->ProgramID);
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstatess);

	if (Alpha) {
		ctxt->enableBlend(true);
		ctxt->setBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);
	} else if (Blending) {
		E_BLEND_FACTOR srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact;
		E_MODULATE_FUNC modulate;
		u32 alphaSource;
		unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulate, alphaSource, material.MaterialTypeParam);

		ctxt->setBlendSeparateFunc(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact);
		ctxt->enableBlend(true);
	}

	if (CallBack)
		CallBack->OnSetMaterial(material);
}

void MaterialRenderer::setUniformFloat(const std::string &name, f32 value)
{
    glUniform1f(ShaderObj->getUniformLocation(name), value);
}
void MaterialRenderer::setUniformInt(const std::string &name, s32 value)
{
    glUniform1i(ShaderObj->getUniformLocation(name), value);
}
void MaterialRenderer::setUniformUInt(const std::string &name, u32 value)
{
    glUniform1ui(ShaderObj->getUniformLocation(name), value);
}

void MaterialRenderer::setUniformFloatArray(const std::string &name, std::vector<f32> values)
{
    glUniform1fv(ShaderObj->getUniformLocation(name), values.size(), values.data());
}
void MaterialRenderer::setUniformIntArray(const std::string &name, std::vector<s32> values)
{
    glUniform1iv(ShaderObj->getUniformLocation(name), values.size(), values.data());
}
void MaterialRenderer::setUniformUIntArray(const std::string &name, std::vector<u32> values)
{
    glUniform1uiv(ShaderObj->getUniformLocation(name), values.size(), values.data());
}

void MaterialRenderer::setUniform2Float(const std::string &name, core::vector2df value)
{
    glUniform2f(ShaderObj->getUniformLocation(name), value.X, value.Y);
}
void MaterialRenderer::setUniform2Int(const std::string &name, core::vector2di value)
{
    glUniform2i(ShaderObj->getUniformLocation(name), value.X, value.Y);
}
void MaterialRenderer::setUniform2UInt(const std::string &name, core::vector2du value)
{
    glUniform2ui(ShaderObj->getUniformLocation(name), value.X, value.Y);
}

void MaterialRenderer::setUniform3Float(const std::string &name, core::vector3df value)
{
    glUniform3f(ShaderObj->getUniformLocation(name), value.X, value.Y, value.Z);
}
void MaterialRenderer::setUniform3Int(const std::string &name, core::vector3di value)
{
    glUniform3i(ShaderObj->getUniformLocation(name), value.X, value.Y, value.Z);
}
void MaterialRenderer::setUniform3UInt(const std::string &name, core::vector3du value)
{
    glUniform3ui(ShaderObj->getUniformLocation(name), value.X, value.Y, value.Z);
}

void MaterialRenderer::setUniform4Float(const std::string &name, f32 value[4])
{
	glUniform4f(ShaderObj->getUniformLocation(name), value[0], value[1], value[2], value[3]);
}
void MaterialRenderer::setUniform4Int(const std::string &name, s32 value[4])
{
	glUniform4i(ShaderObj->getUniformLocation(name), value[0], value[1], value[2], value[3]);
}
void MaterialRenderer::setUniform4UInt(const std::string &name, u32 value[4])
{
	glUniform4ui(ShaderObj->getUniformLocation(name), value[0], value[1], value[2], value[3]);
}

void MaterialRenderer::setUniform4x4Matrix(const std::string &name, core::matrix4 value)
{
    glUniformMatrix4fv(ShaderObj->getUniformLocation(name), 1, GL_FALSE, value.pointer());
}

void MaterialRenderer::setUniformFloatStruct(const std::string &name, const std::unordered_map<std::string, f32> &values)
{
	for (const auto &value : values) {
		std::string uniform_name = name + "." + value.first;

		setUniformFloat(uniform_name, value.second);
	}
}

void MaterialRenderer::setUniformColorfRGB(const std::string &name, const SColorf &colorf)
{
    glUniform3f(ShaderObj->getUniformLocation(name), colorf.r, colorf.g, colorf.b);
}

void MaterialRenderer::setUniformColorfRGBA(const std::string &name, const SColorf &colorf)
{
    glUniform4f(ShaderObj->getUniformLocation(name), colorf.r, colorf.g, colorf.b, colorf.a);
}

VideoDriver *MaterialRenderer::getVideoDriver()
{
	return Driver;
}

}
