#include "PixelFormats.h"

namespace img
{

// Removed A1R5G5B5 and R5G6B5 formats
toGLFormatConverter[PF_RGB8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 24};                    // WARNING: may not be renderable
toGLFormatConverter[PF_RGBA8] = {GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 32};
toGLFormatConverter[PF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT, 16};
toGLFormatConverter[PF_RG16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT, 32};
toGLFormatConverter[PF_RGBA16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 64};
toGLFormatConverter[PF_R32F] = {GL_R32F, GL_RED, GL_FLOAT, 32};
toGLFormatConverter[PF_RG32F] = {GL_RG32F, GL_RG, GL_FLOAT, 64};
toGLFormatConverter[PF_RGBA32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT, 128};
toGLFormatConverter[PF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE, 8};
toGLFormatConverter[PF_RG8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, 16};
toGLFormatConverter[PF_R16] = {GL_R16, GL_RED, GL_UNSIGNED_SHORT, 16};
toGLFormatConverter[PF_RG16] = {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, 32};
toGLFormatConverter[PF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 16};
toGLFormatConverter[PF_D32] = {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 32}; // WARNING: may not be renderable (?!)
toGLFormatConverter[PF_D24S8] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 32};


//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(PixelFormat format, u32 width, u32 height)
{
	// non-compressed formats
	u32 imageSize = toGLFormatConverter.at(format).size / 8 * width;
	imageSize *= height;

	return imageSize;
}

//! check if the pixel format is only viable for depth/stencil textures
bool isDepthFormat(PixelFormat format)
{
	switch (format) {
	case PF_D16:
	case PF_D32:
	case PF_D24S8:
		return true;
	default:
		return false;
	}
}

//! Check if the pixel format uses floating point values for pixels
bool isFloatingPointFormat(PixelFormat format)
{
	switch (format) {
	case PF_R16F:
	case PF_RG16F:
	case PF_RGBA16F:
	case PF_R32F:
	case PF_RG32F:
	case PF_RGBA32F:
		return true;
	default:
		break;
	}
	return false;
}

}
