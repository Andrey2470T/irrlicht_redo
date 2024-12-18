#include "PixelFormats.h"

namespace img
{

// Removed A1R5G5B5 and R5G6B5 formats
pixelFormatInfo[PF_RGB8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, BasicType::UINT8, 24, 3};                    // WARNING: may not be renderable
pixelFormatInfo[PF_RGBA8] = {GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, BasicType::UINT8, 32, 4, true};
pixelFormatInfo[PF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT, BasicType::FLOAT, 16, 1, false, false, true};
pixelFormatInfo[PF_RG16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT, BasicType::FLOAT, 32, 2, false, false, true};
pixelFormatInfo[PF_RGBA16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, BasicType::FLOAT, 64, 4, true, false, true};
pixelFormatInfo[PF_R32F] = {GL_R32F, GL_RED, GL_FLOAT, BasicType::FLOAT, 32, 1, false, false, true};
pixelFormatInfo[PF_RG32F] = {GL_RG32F, GL_RG, GL_FLOAT, BasicType::FLOAT, 64, 2, false, false, true};
pixelFormatInfo[PF_RGBA32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT, BasicType::FLOAT, 128, 4, true, false, true};
pixelFormatInfo[PF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE, BasicType::UINT8, 8, 1};
pixelFormatInfo[PF_RG8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, BasicType::UINT8, 16, 2};
pixelFormatInfo[PF_R16] = {GL_R16, GL_RED, GL_UNSIGNED_SHORT, BasicType::UINT16, 16, 1};
pixelFormatInfo[PF_RG16] = {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, BasicType::UINT16, 32, 2};
pixelFormatInfo[PF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, BasicType::UINT16, 16, 1, false, true};
pixelFormatInfo[PF_D32] = {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, BasicType::UINT32, 32, 1, false, true}; // WARNING: may not be renderable (?!)
pixelFormatInfo[PF_DS16] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, BasicType::UINT16, 32, 2, false, true};
pixelFormatInfo[PF_INDEX_RGBA8] = {0, 0, 0, BasicType::UINT8, 8, 1, false, false, false, true};


//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(PixelFormat format, u32 width, u32 height)
{
	return pixelFormatInfo[format].size / 8 * width * height;
}

u32 getRedMask(PixelFormat format)
{
	switch (format) {
		case PF_RGB8:
			return 0xFF0000;
		case PF_RGBA8:
			return 0xFF000000;
		default:
			return 0x0;
	}
}

u32 getGreenMask(PixelFormat format)
{
	switch (format) {
		case PF_RGB8:
			return 0x00FF00;
		case PF_RGBA8:
			return 0x00FF0000;
		default:
			return 0x0;
	}
}

u32 getBlueMask(PixelFormat format)
{
	switch (format) {
		case PF_RGB8:
			return 0x0000FF;
		case PF_RGBA8:
			return 0x0000FF00;
		default:
			return 0x0;
	}
}

u32 getAlphaMask(PixelFormat format)
{
	switch (format) {
		case PF_RGBA8:
			return 0x000000FF;
		default:
			return 0x0;
	}
}

}
