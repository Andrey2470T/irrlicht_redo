// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "MaterialCallbacks.h"
#include "Device/Logger.h"

#include "Video/VideoDriver.h"


namespace video
{

// Base callback
void MaterialBaseCB::OnSetMaterial(const SMaterial &material)
{
	FogEnable = material.FogEnable;
	Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
}

void MaterialBaseCB::OnSetUniforms(MaterialRenderer *renderer)
{
	VideoDriver *driver = renderer->getVideoDriver();

	const core::matrix4 &W = driver->getTransform(ETS_WORLD);
	const core::matrix4 &V = driver->getTransform(ETS_VIEW);
	const core::matrix4 &P = driver->getTransform(ETS_PROJECTION);

	core::matrix4 Matrix = V * W;
    renderer->setUniform4x4Matrix("uWVMatrix", Matrix);

	Matrix = P * Matrix;
    renderer->setUniform4x4Matrix("uWVPMatrix", Matrix);

	s32 TempEnable = FogEnable ? 1 : 0;
    renderer->setUniformInt("uFogEnable", TempEnable);

	if (FogEnable) {
		SColor TempColor(0);
		E_FOG_TYPE TempType = EFT_FOG_LINEAR;
		f32 FogStart, FogEnd, FogDensity;
		bool unused = false;

		driver->getFog(TempColor, TempType, FogStart, FogEnd, FogDensity, unused, unused);

		s32 FogType = (s32)TempType;
		SColorf FogColor(TempColor);

        renderer->setUniformInt("uFogType", FogType);
        renderer->setUniformColorfRGBA("uFogColor", FogColor);
        renderer->setUniformFloat("uFogStart", FogStart);
        renderer->setUniformFloat("uFogEnd", FogEnd);
        renderer->setUniformFloat("uFogDensity", FogDensity);
	}

    renderer->setUniformFloat("uThickness", Thickness);
}

// EMT_SOLID + EMT_TRANSPARENT_ALPHA_CHANNEL + EMT_TRANSPARENT_VERTEX_ALPHA

void MaterialSolidCB::OnSetMaterial(const SMaterial &material)
{
	MaterialBaseCB::OnSetMaterial(material);

	AlphaRef = material.MaterialTypeParam;
	TextureUsage0 = (material.TextureLayers[0].Texture) ? 1 : 0;
}

void MaterialSolidCB::OnSetUniforms(MaterialRenderer *renderer)
{
	MaterialBaseCB::OnSetUniforms(renderer);

    VideoDriver *driver = renderer->getVideoDriver();

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
    renderer->setUniform4x4Matrix("uTMatrix0", Matrix);

    renderer->setUniformFloat("uAlphaRef", AlphaRef);
    renderer->setUniformInt("uTextureUsage0", TextureUsage0);
    renderer->setUniformInt("uTextureUnit0", TextureUnit0);
}

// EMT_ONETEXTURE_BLEND

void MaterialOneTextureBlendCB::OnSetMaterial(const SMaterial &material)
{
	MaterialBaseCB::OnSetMaterial(material);

	BlendType = 0;

	E_BLEND_FACTOR srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact;
	E_MODULATE_FUNC modulate;
	u32 alphaSource;
	unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulate, alphaSource, material.MaterialTypeParam);

	if (textureBlendFunc_hasAlpha(srcRGBFact) || textureBlendFunc_hasAlpha(dstRGBFact) || textureBlendFunc_hasAlpha(srcAlphaFact) || textureBlendFunc_hasAlpha(dstAlphaFact)) {
		if (alphaSource == EAS_VERTEX_COLOR) {
			BlendType = 1;
		} else if (alphaSource == EAS_TEXTURE) {
			BlendType = 2;
		}
	}

	TextureUsage0 = (material.TextureLayers[0].Texture) ? 1 : 0;
}

void MaterialOneTextureBlendCB::OnSetUniforms(MaterialRenderer *renderer)
{
	MaterialBaseCB::OnSetUniforms(renderer);

	VideoDriver *driver = renderer->getVideoDriver();

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
    renderer->setUniform4x4Matrix("uTMatrix0", Matrix);

    renderer->setUniformInt("uBlendType", BlendType);
    renderer->setUniformInt("uTextureUsage0", TextureUsage0);
    renderer->setUniformInt("uTextureUnit0", TextureUnit0);
}

void Material2DCB::OnSetMaterial(const SMaterial &material)
{
	Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
	TextureUsage0 = material.TextureLayers[0].Texture ? 1 : 0;
}

void Material2DCB::OnSetUniforms(MaterialRenderer *renderer)
{
	renderer->setUniformFloat("uThickness", Thickness);

	// Update projection matrix
	VideoDriver *driver = renderer->getVideoDriver();
	const core::dimension2d<u32> renderTargetSize = driver->getCurrentRenderTargetSize();
	core::matrix4 proj;
	float xInv2 = 2.0f / renderTargetSize.Width;
	float yInv2 = 2.0f / renderTargetSize.Height;
	proj.setScale({ xInv2, -yInv2, 0.0f });
	proj.setTranslation({ -1.0f, 1.0f, 0.0f });
	renderer->setUniform4x4Matrix("uProjection", proj);

	renderer->setUniformInt("uTextureUnit", 0);
	renderer->setUniformInt("uTextureUsage", TextureUsage0);
}

}
