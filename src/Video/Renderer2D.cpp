// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Renderer2D.h"

#include "Logger.h"

#include "VideoDriver.h"
#include "DrawContext.h"
#include "RenderTarget.h"
#include "Texture.h"


namespace video
{

COpenGL3Renderer2D::COpenGL3Renderer2D(const c8 *vertexShaderProgram, const c8 *pixelShaderProgram, VideoDriver *driver, bool withTexture) :
		MaterialRenderer(driver, 0, EMT_SOLID),
		WithTexture(withTexture)
{
	int Temp = 0;
	init(Temp, vertexShaderProgram, pixelShaderProgram,
		withTexture ? "2DTexture" : "2DNoTexture", false);

	auto ctxt = Driver->getContext();

    ctxt->setProgram(ShaderObj->ProgramID);

	// These states don't change later.
	if (WithTexture) {
        setUniformInt("uTextureUnit", 0);
        setUniformInt("uTextureUsage", 0);
	}

    ctxt->setProgram(0);
}

void COpenGL3Renderer2D::OnSetMaterial(const video::SMaterial &material,
		const video::SMaterial &lastMaterial,
		bool resetAllRenderstates)
{
    Driver->getContext()->setProgram(ShaderObj->ProgramID);
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	f32 Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
    setUniformFloat("uThickness", Thickness);

	{
		// Update projection matrix
		const core::dimension2d<u32> renderTargetSize = Driver->getCurrentRenderTargetSize();
		core::matrix4 proj;
		float xInv2 = 2.0f / renderTargetSize.Width;
		float yInv2 = 2.0f / renderTargetSize.Height;
		proj.setScale({ xInv2, -yInv2, 0.0f });
		proj.setTranslation({ -1.0f, 1.0f, 0.0f });
        setUniform4x4Matrix("uProjection", proj);
	}

	if (WithTexture) {
		s32 TextureUsage = material.TextureLayers[0].Texture ? 1 : 0;
        setUniformInt("uTextureUsage", TextureUsage);
	}
}

bool COpenGL3Renderer2D::OnRender(E_VERTEX_TYPE vtxtype)
{
	return true;
}

}
