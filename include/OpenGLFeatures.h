#pragma once

#include "irrTypes.h"

namespace video
{

struct OpenGLFeatures
{
	bool BlendOperation = false;
	bool TexStorage = false;

	u8 ColorAttachment = 0;
	u8 MultipleRenderTarget = 0;
	u8 MaxTextureUnits = 0;
	u8 MaxAnisotropy = 1;
	u32 MaxIndices = 0xffff;
	u32 MaxTextureSize = 1;
	u32 MaxArrayTextureLayers = 1;
	f32 MaxTextureLODBias = 0.0f;
	//! Minimal and maximal supported thickness for lines without smoothing
	float DimAliasedLine[2] = {1.0f, 1.0f};
	//! Minimal and maximal supported thickness for points without smoothing
	float DimAliasedPoint[2] = {1.0f, 1.0f};
	bool StencilBuffer = false;
	bool LODBiasSupported = false;
	bool AnisotropicFilterSupported = false;
	bool BlendMinMaxSupported = false;
	bool TextureMultisampleSupported = false;
	bool Texture2DArraySupported = false;
	bool KHRDebugSupported = false;
	u32 MaxLabelLength = 0;
};

enum OpenGLSpec : u8
{
	Core,
	Compat,
	ES,
	// WebGL, // TODO
};

struct OpenGLVersion
{
	OpenGLSpec Spec;
	u8 Major;
	u8 Minor;
	u8 Release;
};

}
