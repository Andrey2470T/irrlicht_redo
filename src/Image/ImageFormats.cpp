#include "ImageFormats.h"

namespace img
{

// Removed A1R5G5B5 and R5G6B5 formats
toGLFormatConverter[IF_RGB8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 24};                    // WARNING: may not be renderable
toGLFormatConverter[IF_RGBA8] = {GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 32};
toGLFormatConverter[IF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT, 16};
toGLFormatConverter[IF_RG16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT, 32};
toGLFormatConverter[IF_RGBA16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 64};
toGLFormatConverter[IF_R32F] = {GL_R32F, GL_RED, GL_FLOAT, 32};
toGLFormatConverter[IF_RG32F] = {GL_RG32F, GL_RG, GL_FLOAT, 64};
toGLFormatConverter[IF_RGBA32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT, 128};
toGLFormatConverter[IF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE, 8};
toGLFormatConverter[IF_RG8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, 16};
toGLFormatConverter[IF_R16] = {GL_R16, GL_RED, GL_UNSIGNED_SHORT, 16};
toGLFormatConverter[IF_RG16] = {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, 32};
toGLFormatConverter[IF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 16};
toGLFormatConverter[IF_D32] = {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 32}; // WARNING: may not be renderable (?!)
toGLFormatConverter[IF_D24S8] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 32};


//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(ImageFormat format, u32 width, u32 height)
{
	// non-compressed formats
	u32 imageSize = toGLFormatConverter.at(format).size / 8 * width;
	imageSize *= height;

	return imageSize;
}

//! check if the color format is only viable for depth/stencil textures
bool isDepthFormat(ImageFormat format)
{
	switch (format) {
	case IF_D16:
	case IF_D32:
	case IF_D24S8:
		return true;
	default:
		return false;
	}
}

//! Check if the color format uses floating point values for pixels
bool isFloatingPointFormat(ImageFormat format)
{
	switch (format) {
	case IF_R16F:
	case IF_RG16F:
	case IF_RGBA16F:
	case IF_R32F:
	case IF_RG32F:
	case IF_RGBA32F:
		return true;
	default:
		break;
	}
	return false;
}

}
