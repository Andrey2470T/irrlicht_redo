#pragma once

#include <array>
#include "Common.h"

namespace render
{

enum ImageFormat
{
	IF_R8G8B8 = 0,
	IF_A8R8G8B8,
	IF_R16F,
	IF_G16R16F,
	IF_A16B16G16R16F,
	IF_R32F,
	IF_G32R32F,
	IF_A32B32G32R32F,
	IF_R8,
	IF_R8G8,
	IF_R16,
	IF_R16G16,
	IF_D16,
	IF_D32,
	IF_D24S8,
	IF_COUNT
};

struct ImageFormatInfo
{
	GLenum internalFormat;
	GLenum pixelFormat;
	GLenum pixelType;
	u32 size;
};

extern std::array<ImageFormatInfo, IF_COUNT> toGLFormatConverter;

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(ImageFormat format, u32 width, u32 height);

//! check if the image format is only viable for depth/stencil textures
static bool isDepthFormat(ImageFormat format);

//! Check if the image color format uses floating point values for pixels
static bool isFloatingPointFormat(ImageFormat format);

}
