// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include "MaterialRenderer.h"


namespace video
{

class COpenGL3Renderer2D : public MaterialRenderer
{
public:
	COpenGL3Renderer2D(const c8 *vertexShaderProgram, const c8 *pixelShaderProgram, VideoDriver *driver, bool withTexture);

	virtual void OnSetMaterial(const SMaterial &material, const SMaterial &lastMaterial,
			bool resetAllRenderstates);

	virtual bool OnRender(E_VERTEX_TYPE vtxtype);

protected:
	bool WithTexture;
};

}
