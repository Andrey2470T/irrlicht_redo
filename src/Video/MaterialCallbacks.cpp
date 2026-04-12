// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2014 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "MaterialCallbacks.h"
#include "Logger.h"

#include "VideoDriver.h"


namespace video
{

// Base callback
void MaterialBaseCB::OnSetMaterial(const SMaterial &material)
{
	FogEnable = material.FogEnable;
	Thickness = (material.Thickness > 0.f) ? material.Thickness : 1.f;
}

void MaterialBaseCB::OnSetConstants(MaterialRenderer *renderer, s32 userData)
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
        renderer->setUniformColorf("uFogColor", FogColor);
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

void MaterialSolidCB::OnSetConstants(MaterialRenderer *renderer, s32 userData)
{
	MaterialBaseCB::OnSetConstants(renderer, userData);

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

void MaterialOneTextureBlendCB::OnSetConstants(MaterialRenderer *renderer, s32 userData)
{
	MaterialBaseCB::OnSetConstants(renderer, userData);

	VideoDriver *driver = renderer->getVideoDriver();

	core::matrix4 Matrix = driver->getTransform(ETS_TEXTURE_0);
    renderer->setUniform4x4Matrix("uTMatrix0", Matrix);

    renderer->setUniformInt("uBlendType", BlendType);
    renderer->setUniformInt("uTextureUsage0", TextureUsage0);
    renderer->setUniformInt("uTextureUnit0", TextureUnit0);
}

}
