#pragma once

#include "Render/Common.h"

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
	PF_INDEX_RGBA8,
	PF_COUNT
};

struct PixelFormatInfo
{
	GLenum internalFormat;
	GLenum pixelFormat;
	GLenum pixelType;

	BasicType type;
	u8 size;
	u8 channels;

    bool hasAlpha = false;
	bool isDepth = false;
	bool isFloatingPoint = false;
	bool isIndexed = false;
};

// Removed A1R5G5B5 and R5G6B5 formats
extern const std::array<PixelFormatInfo, PF_COUNT> pixelFormatInfo;

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(PixelFormat format, u32 width, u32 height);

u32 getRedMask(PixelFormat format);
u32 getGreenMask(PixelFormat format);
u32 getBlueMask(PixelFormat format);
u32 getAlphaMask(PixelFormat format);

}
