// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "Renderer2D.h"

#include "Logger.h"

#include "VideoDriver.h"
#include "DrawContext.h"
#include "RenderTarget.h"

#include "Video/COpenGLCoreTexture.h"


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

	ctxt->setProgram(Program);

	// These states don't change later.

	ProjectionID = getPixelShaderConstantID("uProjection");
	ThicknessID = getPixelShaderConstantID("uThickness");
	if (WithTexture) {
		TextureUsageID = getPixelShaderConstantID("uTextureUsage");
		s32 TextureUnitID = getPixelShaderConstantID("uTextureUnit");

		s32 TextureUnit = 0;
		setPixelShaderConstant(TextureUnitID, &TextureUnit, 1);

		s32 TextureUsage = 0;
		setPixelShaderConstant(TextureUsageID, &TextureUsage, 1);
	}

	ctxt->setProgram(0);
}

COpenGL3Renderer2D::~COpenGL3Renderer2D()
{
}

void COpenGL3Renderer2D::OnSetMaterial(const video::SMaterial &material,
		const video::SMaterial &lastMaterial,
		bool resetAllRenderstates)
{
	Driver->getContext()->setProgram(Program);
	Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

	f32 Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
	setPixelShaderConstant(ThicknessID, &Thickness, 1);

	{
		// Update projection matrix
		const core::dimension2d<u32> renderTargetSize = Driver->getCurrentRenderTargetSize();
		core::matrix4 proj;
		float xInv2 = 2.0f / renderTargetSize.Width;
		float yInv2 = 2.0f / renderTargetSize.Height;
		proj.setScale({ xInv2, -yInv2, 0.0f });
		proj.setTranslation({ -1.0f, 1.0f, 0.0f });
		setPixelShaderConstant(ProjectionID, proj.pointer(), 4 * 4);
	}

	if (WithTexture) {
		s32 TextureUsage = material.TextureLayers[0].Texture ? 1 : 0;
		setPixelShaderConstant(TextureUsageID, &TextureUsage, 1);
	}
}

bool COpenGL3Renderer2D::OnRender(E_VERTEX_TYPE vtxtype)
{
	return true;
}

}
