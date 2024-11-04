#include "ColorFormats.h"

namespace render
{

toGLFormatConverter[CF_A1R5G5B5] = {GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, 16}; // WARNING: may not be renderable
toGLFormatConverter[CF_R5G6B5] = {GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 16};              // GL_RGB565 is an extension until 4.1
toGLFormatConverter[CF_R8G8B8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, 24};                    // WARNING: may not be renderable
toGLFormatConverter[CF_A8R8G8B8] = {GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 32};
toGLFormatConverter[CF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT, 16};
toGLFormatConverter[CF_G16R16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT, 32};
toGLFormatConverter[CF_A16B16G16R16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, 64};
toGLFormatConverter[CF_R32F] = {GL_R32F, GL_RED, GL_FLOAT, 32};
toGLFormatConverter[CF_G32R32F] = {GL_RG32F, GL_RG, GL_FLOAT, 64};
toGLFormatConverter[CF_A32B32G32R32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT, 128};
toGLFormatConverter[CF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE, 8};
toGLFormatConverter[CF_R8G8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, 16};
toGLFormatConverter[CF_R16] = {GL_R16, GL_RED, GL_UNSIGNED_SHORT, 16};
toGLFormatConverter[CF_R16G16] = {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, 32};
toGLFormatConverter[CF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, 16};
toGLFormatConverter[CF_D32] = {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 32}; // WARNING: may not be renderable (?!)
toGLFormatConverter[CF_D24S8] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 32};


//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(ColorFormat format, u32 width, u32 height)
{
	// non-compressed formats
	u32 imageSize = toGLFormatConverter.at(format).size / 8 * width;
	imageSize *= height;

	return imageSize;
}

}
