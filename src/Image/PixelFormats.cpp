#include "Image/PixelFormats.h"

namespace video
{

const std::array<PixelFormatInfo, ECF_UNKNOWN+1> pixelFormatsInfo = {{
	{"A1R5G5B5", 16, true},
	{"R5G6B5", 16},
	{"R8G8B8", 24},
	{"A8R8G8B8", 32, true},
	{"R16F", 16, false, false, true},
	{"G16R16F", 32, false, false, true},
	{"A16B16G16R16F", 64, true, false, true},
	{"R32F", 32, false, false, true},
	{"G32R32F", 64, false, false, true},
	{"A32B32G32R32F", 128, true, false, true},
	{"R8", 8},
	{"R8G8", 16},
	{"R16", 16},
	{"R16G16", 32},
	{"A2R10G10B10", 32, true},
	{"D16", 16, false, true},
	{"D24", 32, false, true},
	{"D32", 32, false, true},
	{"D24S8", 32, false, true},
	{"UNKNOWN", 0}
}};

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(ECOLOR_FORMAT format, u32 width, u32 height)
{
	// non-compressed formats
	u32 imageSize = pixelFormatsInfo[format].size / 8 * width;
	imageSize *= height;

	return imageSize;
}

//! Returns mask for red value of a pixel
u32 getRedMask(ECOLOR_FORMAT format)
{
	switch (format) {
	case ECF_A1R5G5B5:
		return 0x1F << 10;
	case ECF_R5G6B5:
		return 0x1F << 11;
	case ECF_R8G8B8:
		return 0x00FF0000;
	case ECF_A8R8G8B8:
		return 0x00FF0000;
	default:
		return 0x0;
	}
}

//! Returns mask for green value of a pixel
u32 getGreenMask(ECOLOR_FORMAT format)
{
	switch (format) {
	case ECF_A1R5G5B5:
		return 0x1F << 5;
	case ECF_R5G6B5:
		return 0x3F << 5;
	case ECF_R8G8B8:
		return 0x0000FF00;
	case ECF_A8R8G8B8:
		return 0x0000FF00;
	default:
		return 0x0;
	}
}

//! Returns mask for blue value of a pixel
u32 getBlueMask(ECOLOR_FORMAT format)
{
	switch (format) {
	case ECF_A1R5G5B5:
		return 0x1F;
	case ECF_R5G6B5:
		return 0x1F;
	case ECF_R8G8B8:
		return 0x000000FF;
	case ECF_A8R8G8B8:
		return 0x000000FF;
	default:
		return 0x0;
	}
}

//! Returns mask for alpha value of a pixel
u32 getAlphaMask(ECOLOR_FORMAT format)
{
	switch (format) {
	case ECF_A1R5G5B5:
		return 0x1 << 15;
	case ECF_R5G6B5:
		return 0x0;
	case ECF_R8G8B8:
		return 0x0;
	case ECF_A8R8G8B8:
		return 0xFF000000;
	default:
		return 0x0;
	}
}

}
