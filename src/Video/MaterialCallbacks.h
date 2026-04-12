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
	virtual void OnSetMaterial(const SMaterial &material);
	virtual void OnSetConstants(MaterialRenderer *renderer, s32 userData);

protected:
    f32 Thickness = 1.0f;
    bool FogEnable = false;
};

class MaterialSolidCB : public MaterialBaseCB
{
public:
	virtual void OnSetMaterial(const SMaterial &material);
	virtual void OnSetConstants(MaterialRenderer *renderer, s32 userData);

protected:
    f32 AlphaRef = 0.5f;
    s32 TextureUsage0 = 0;
    s32 TextureUnit0 = 0;
};

class MaterialOneTextureBlendCB : public MaterialBaseCB
{
public:
	virtual void OnSetMaterial(const SMaterial &material);
	virtual void OnSetConstants(MaterialRenderer *renderer, s32 userData);

protected:
    s32 BlendType = 0;
    s32 TextureUsage0 = 0;
    s32 TextureUnit0 = 0;
};

}
