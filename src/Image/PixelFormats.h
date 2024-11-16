#pragma once

#include "Render/Common.h"

using namespace utils;

namespace img
{

enum PixelFormat
{
	PF_RGB8 = 0,
	PF_RGBA8,
	PF_R16F,
	PF_RG16F,
	PF_RGBA16F,
	PF_R32F,
	PF_RG32F,
	PF_RGBA32F,
	PF_R8,
	PF_RG8,
	PF_R16,
	PF_RG16,
	PF_D16,
	PF_D32,
	PF_D24S8,
	PF_COUNT
};

struct PixelFormatInfo
{
	GLenum internalFormat;
	GLenum pixelFormat;
	GLenum pixelType;
	u32 size;
};

extern std::array<PixelFormatInfo, IF_COUNT> toGLFormatConverter;

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(PixelFormat format, u32 width, u32 height);

//! check if the pixel format is only viable for depth/stencil textures
static bool isDepthFormat(PixelFormat format);

//! Check if the pixel format uses floating point values for pixels
static bool isFloatingPointFormat(PixelFormat format);

}
