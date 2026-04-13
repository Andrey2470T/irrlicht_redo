// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include "IShaderConstantSetCallBack.h"
#include "MaterialRenderer.h"


namespace video
{

class MaterialBaseCB : public IShaderConstantSetCallBack
{
public:
	void OnSetMaterial(const SMaterial &material) override;
	void OnSetConstants(MaterialRenderer *renderer, s32 userData) override;

protected:
    f32 Thickness = 1.0f;
    bool FogEnable = false;
};

class MaterialSolidCB : public MaterialBaseCB
{
public:
	void OnSetMaterial(const SMaterial &material) override;
	void OnSetConstants(MaterialRenderer *renderer, s32 userData) override;

protected:
    f32 AlphaRef = 0.5f;
    s32 TextureUsage0 = 0;
    s32 TextureUnit0 = 0;
};

class MaterialOneTextureBlendCB : public MaterialBaseCB
{
public:
	void OnSetMaterial(const SMaterial &material) override;
	void OnSetConstants(MaterialRenderer *renderer, s32 userData) override;

protected:
    s32 BlendType = 0;
    s32 TextureUsage0 = 0;
    s32 TextureUnit0 = 0;
};

class Material2DCB : public IShaderConstantSetCallBack
{
public:
	void OnSetMaterial(const SMaterial &material) override;
	void OnSetConstants(MaterialRenderer *renderer, s32 userData) override;

private:
	f32 Thickness = 1.0f;
	s32 TextureUsage0 = 0;
};

}
