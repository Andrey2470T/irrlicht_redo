#pragma once

#include <array>
#include "Common.h"

namespace render
{

enum TextureFormat
{
	TF_A1R5G5B5 = 0,
	TF_R5G6B5,
	TF_R8G8B8,
	TF_A8R8G8B8,
	TF_R16F,
	TF_G16R16F,
	TF_A16B16G16R16F,
	TF_R32F,
	TF_G32R32F,
	TF_A32B32G32R32F,
	TF_R8,
	TF_R8G8,
	TF_R16,
	TF_R16G16,
	TF_D16,
	TF_D32,
	TF_D24S8,
	TF_COUNT
};

struct TextureFormatInfo
{
	GLenum internalFormat;
	GLenum pixelFormat;
	GLenum pixelType;
};

extern std::array<TextureFormatInfo, TF_COUNT> toGLFormatConverter;

}

