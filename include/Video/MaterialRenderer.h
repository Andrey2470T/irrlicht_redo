// Copyright (C) 2002-2012 Nikolaus Gebhardt
// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <string>
#include <vector>
#include "Enums/EMaterialTypes.h"
#include "Utils/IReferenceCounted.h"
#include "Video/SMaterial.h"
#include "Mesh/VertexTypes.h"
#include "Utils/irrArray.h"
#include "SOverrideMaterial.h"
#include <memory>
#include <unordered_map>

namespace video
{

class VideoDriver;
class IShaderConstantSetCallBack;
class MaterialSystem;

enum E_SHADER_TYPE
{
    EST_VERTEX = 0,
    EST_GEOMETRY,
    EST_FRAGMENT
};

class Shader
{
    u32 ProgramID = 0;
    u32 VertexShaderID = 0;
    u32 GeometryShaderID = 0;
    u32 FragmentShaderID = 0;

    std::unordered_map<std::string, u32> Uniforms;
public:
    Shader(
        const std::string &vertexShaderCode,
        const std::string &fragmentShaderCode,
        const std::string &geometryShaderCode = "");

    ~Shader();

private:
    u32 createShader(E_SHADER_TYPE, const std::string &code);
    void createProgram();
    s32 getUniformLocation(const std::string &name);

    friend class MaterialRenderer;
};

class MaterialRenderer : public IReferenceCounted
{
public:
	MaterialRenderer(
			VideoDriver *driver,
			s32 &outMaterialTypeNr,
            const std::string &vertexShaderProgram = "",
            const std::string &fragmentShaderProgram = "",
            const std::string &debugName = "",
			IShaderConstantSetCallBack *callback = 0,
			E_MATERIAL_TYPE baseMaterial = EMT_SOLID,
			s32 userData = 0);

    ~MaterialRenderer();

	//! Called by the IVideoDriver implementation to set needed render states.
    void OnSetMaterial(const SMaterial &material, const SMaterial &lastMaterial,
			bool resetAllRenderstates);

	//! Called every time before a new bunch of geometry is drawn.
	bool OnRender(scene::E_VERTEX_TYPE vtxtype);

	//! Returns if the material is transparent.
	bool isTransparent() const
	{
		return (Alpha || Blending);
	}

    void setUniformFloat(const std::string &name, f32 value);
    void setUniformInt(const std::string &name, s32 value);
    void setUniformUInt(const std::string &name, u32 value);

    void setUniformFloatArray(const std::string &name, std::vector<f32> values);
    void setUniformIntArray(const std::string &name, std::vector<s32> values);
    void setUniformUIntArray(const std::string &name, std::vector<u32> values);

    void setUniform2Float(const std::string &name, core::vector2df value);
    void setUniform2Int(const std::string &name, core::vector2di value);
    void setUniform2UInt(const std::string &name, core::vector2du value);

    void setUniform3Float(const std::string &name, core::vector3df value);
    void setUniform3Int(const std::string &name, core::vector3di value);
    void setUniform3UInt(const std::string &name, core::vector3du value);

	void setUniform4Float(const std::string &name, f32 value[4]);
	void setUniform4Int(const std::string &name, s32 value[4]);
	void setUniform4UInt(const std::string &name, u32 value[4]);

    void setUniform4x4Matrix(const std::string &name, core::matrix4 value);

	void setUniformFloatStruct(const std::string &name, const std::unordered_map<std::string, f32> &values);

    void setUniformColorfRGB(const std::string &name, const SColorf &colorf);
    void setUniformColorfRGBA(const std::string &name, const SColorf &colorf);

	//! Get pointer to the IVideoDriver interface.
    VideoDriver *getVideoDriver();

	//! Access the callback provided by the users when creating shader materials.
    IShaderConstantSetCallBack *getShaderConstantSetCallBack() const;

protected:
    void init(
        s32 &outMaterialTypeNr, const std::string &vertexShaderCode,
		const std::string &fragmentShaderCode, const std::string &debugName = "",
		bool addMaterial = true);

	VideoDriver *Driver;
	IShaderConstantSetCallBack *CallBack;

    std::unique_ptr<Shader> ShaderObj;
	bool Alpha;
	bool Blending;

	s32 UserData;
};

} // end namespace video
