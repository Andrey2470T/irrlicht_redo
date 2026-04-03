// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "MaterialRenderer.h"

#include "EVertexAttributes.h"
#include "IShaderConstantSetCallBack.h"
#include "VideoDriver.h"
#include "Logger.h"

#include "VideoDriver.h"
#include "DrawContext.h"
#include "RenderTarget.h"
#include "Renderer2D.h"
#include "GLSpecificInfo.h"
#include "Texture.h"


namespace video
{

MaterialRenderer::MaterialRenderer(VideoDriver *driver,
		s32 &outMaterialTypeNr,
		const c8 *vertexShaderProgram,
		const c8 *pixelShaderProgram,
		const c8 *debugName,
		IShaderConstantSetCallBack *callback,
		E_MATERIAL_TYPE baseMaterial,
		s32 userData) :
		Driver(driver),
		CallBack(callback), Alpha(false), Blending(false), Program(0), UserData(userData)
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

	init(outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram, debugName);
}

MaterialRenderer::MaterialRenderer(VideoDriver *driver,
		IShaderConstantSetCallBack *callback,
		E_MATERIAL_TYPE baseMaterial, s32 userData) :
		Driver(driver),
		CallBack(callback), Alpha(false), Blending(false), Program(0), UserData(userData)
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
}

MaterialRenderer::~MaterialRenderer()
{
	if (CallBack)
		CallBack->drop();

	if (Program) {
		GLuint shaders[8];
		GLint count;
		glGetAttachedShaders(Program, 8, &count, shaders);

		count = core::min_(count, 8);
		for (GLint i = 0; i < count; ++i)
			glDeleteShader(shaders[i]);
		glDeleteProgram(Program);
		Program = 0;
	}

	UniformInfo.clear();
}

GLuint MaterialRenderer::getProgram() const
{
	return Program;
}

void MaterialRenderer::init(s32 &outMaterialTypeNr,
		const c8 *vertexShaderProgram,
		const c8 *pixelShaderProgram,
		const c8 *debugName,
		bool addMaterial)
{
	outMaterialTypeNr = -1;

	Program = glCreateProgram();

	if (!Program)
		return;

	if (vertexShaderProgram)
		if (!createShader(GL_VERTEX_SHADER, vertexShaderProgram))
			return;

	if (pixelShaderProgram)
		if (!createShader(GL_FRAGMENT_SHADER, pixelShaderProgram))
			return;

	for (size_t i = 0; i < EVA_COUNT; ++i)
		glBindAttribLocation(Program, i, sBuiltInVertexAttributeNames[i]);

	if (!linkProgram())
		return;

	if (debugName)
		Driver->GLInfo->ObjectLabel(GL_PROGRAM, Program, debugName);

	if (addMaterial)
		outMaterialTypeNr = Driver->addMaterialRenderer(this);
}

bool MaterialRenderer::OnRender(E_VERTEX_TYPE vtxtype)
{
	if (CallBack && Program)
		CallBack->OnSetConstants(this, UserData);

	return true;
}

void MaterialRenderer::OnSetMaterial(const video::SMaterial &material,
		const video::SMaterial &lastMaterial,
		bool resetAllRenderstatess, MaterialSystem *materialSys)
{
	auto ctxt = Driver->getContext();

	ctxt->setProgram(Program);

	materialSys->setBasicRenderStates(material, lastMaterial, resetAllRenderstatess);

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

void MaterialRenderer::OnUnsetMaterial()
{
}

bool MaterialRenderer::isTransparent() const
{
	return (Alpha || Blending);
}

s32 MaterialRenderer::getRenderCapability() const
{
	return 0;
}

bool MaterialRenderer::createShader(GLenum shaderType, const char *shader)
{
	if (Program) {
		GLuint shaderHandle = glCreateShader(shaderType);
		glShaderSource(shaderHandle, 1, &shader, NULL);
		glCompileShader(shaderHandle);

		GLint status = 0;

		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

		if (status != GL_TRUE) {
			g_irrlogger->log("GLSL shader failed to compile", ELL_ERROR);

			GLint maxLength = 0;
			GLint length;

			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH,
					&maxLength);

			if (maxLength) {
				GLchar *infoLog = new GLchar[maxLength];
				glGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
				g_irrlogger->log(reinterpret_cast<const c8 *>(infoLog), ELL_ERROR);
				delete[] infoLog;
			}

			return false;
		}

		glAttachShader(Program, shaderHandle);
	}

	return true;
}

bool MaterialRenderer::linkProgram()
{
	if (Program) {
		glLinkProgram(Program);

		GLint status = 0;

		glGetProgramiv(Program, GL_LINK_STATUS, &status);

		if (!status) {
			g_irrlogger->log("GLSL shader program failed to link", ELL_ERROR);

			GLint maxLength = 0;
			GLsizei length;

			glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &maxLength);

			if (maxLength) {
				GLchar *infoLog = new GLchar[maxLength];
				glGetProgramInfoLog(Program, maxLength, &length, infoLog);
				g_irrlogger->log(reinterpret_cast<const c8 *>(infoLog), ELL_ERROR);
				delete[] infoLog;
			}

			return false;
		}

		GLint num = 0;

		glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &num);

		if (num == 0)
			return true;

		GLint maxlen = 0;

		glGetProgramiv(Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

		if (maxlen == 0) {
			g_irrlogger->log("GLSL: failed to retrieve uniform information", ELL_ERROR);
			return false;
		}

		// seems that some implementations use an extra null terminator.
		++maxlen;
		std::vector<c8> buf(maxlen);

		UniformInfo.clear();
		UniformInfo.reserve(num);

		for (GLint i = 0; i < num; ++i) {
			SUniformInfo ui;
			memset(buf.data(), 0, buf.size());

			GLint size;
			glGetActiveUniform(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLchar *>(buf.data()));

			// array support, workaround for some bugged drivers.
			for (s32 i = 0; i < maxlen; ++i) {
				if (buf[i] == '[' || buf[i] == '\0')
					break;

				ui.name += buf[i];
			}

			ui.location = glGetUniformLocation(Program, buf.data());

			UniformInfo.push_back(std::move(ui));
		}
	}

	return true;
}

s32 MaterialRenderer::getVertexShaderConstantID(const c8 *name)
{
	return getPixelShaderConstantID(name);
}

s32 MaterialRenderer::getPixelShaderConstantID(const c8 *name)
{
	for (u32 i = 0; i < UniformInfo.size(); ++i) {
		if (UniformInfo[i].name == name)
			return i;
	}

	return -1;
}

bool MaterialRenderer::setVertexShaderConstant(s32 index, const f32 *floats, int count)
{
	return setPixelShaderConstant(index, floats, count);
}

bool MaterialRenderer::setVertexShaderConstant(s32 index, const s32 *ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

bool MaterialRenderer::setVertexShaderConstant(s32 index, const u32 *ints, int count)
{
	return setPixelShaderConstant(index, ints, count);
}

bool MaterialRenderer::setPixelShaderConstant(s32 index, const f32 *floats, int count)
{
	if (index < 0 || UniformInfo[index].location < 0)
		return false;

	bool status = true;

	switch (UniformInfo[index].type) {
	case GL_FLOAT:
		glUniform1fv(UniformInfo[index].location, count, floats);
		break;
	case GL_FLOAT_VEC2:
		glUniform2fv(UniformInfo[index].location, count / 2, floats);
		break;
	case GL_FLOAT_VEC3:
		glUniform3fv(UniformInfo[index].location, count / 3, floats);
		break;
	case GL_FLOAT_VEC4:
		glUniform4fv(UniformInfo[index].location, count / 4, floats);
		break;
	case GL_FLOAT_MAT2:
		glUniformMatrix2fv(UniformInfo[index].location, count / 4, false, floats);
		break;
	case GL_FLOAT_MAT3:
		glUniformMatrix3fv(UniformInfo[index].location, count / 9, false, floats);
		break;
	case GL_FLOAT_MAT4:
		glUniformMatrix4fv(UniformInfo[index].location, count / 16, false, floats);
		break;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_CUBE: {
		if (floats) {
			const GLint id = (GLint)(*floats);
			glUniform1iv(UniformInfo[index].location, 1, &id);
		} else
			status = false;
	} break;
	default:
		status = false;
		break;
	}

	return status;
}

bool MaterialRenderer::setPixelShaderConstant(s32 index, const s32 *ints, int count)
{
	if (index < 0 || UniformInfo[index].location < 0)
		return false;

	bool status = true;

	switch (UniformInfo[index].type) {
	case GL_INT:
	case GL_BOOL:
		glUniform1iv(UniformInfo[index].location, count, ints);
		break;
	case GL_INT_VEC2:
	case GL_BOOL_VEC2:
		glUniform2iv(UniformInfo[index].location, count / 2, ints);
		break;
	case GL_INT_VEC3:
	case GL_BOOL_VEC3:
		glUniform3iv(UniformInfo[index].location, count / 3, ints);
		break;
	case GL_INT_VEC4:
	case GL_BOOL_VEC4:
		glUniform4iv(UniformInfo[index].location, count / 4, ints);
		break;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_CUBE:
		glUniform1iv(UniformInfo[index].location, 1, ints);
		break;
	default:
		status = false;
		break;
	}

	return status;
}

bool MaterialRenderer::setPixelShaderConstant(s32 index, const u32 *ints, int count)
{
	g_irrlogger->log("Unsigned int support is unimplemented", ELL_WARNING);
	return false;
}

VideoDriver *MaterialRenderer::getVideoDriver()
{
	return Driver;
}

}
