#include "PixelFormats.h"

namespace img
{

// Removed A1R5G5B5 and R5G6B5 formats
const std::array<PixelFormatInfo, PF_COUNT> pixelFormatInfo = {{
    {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, BasicType::UINT8, 24, 3},                   // WARNING: may not be renderable
    {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, BasicType::UINT8, 32, 4, true},
    {GL_R16F, GL_RED, GL_HALF_FLOAT, BasicType::FLOAT, 16, 1, false, false, true},
    {GL_RG16F, GL_RG, GL_HALF_FLOAT, BasicType::FLOAT, 32, 2, false, false, true},
    {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, BasicType::FLOAT, 64, 4, true, false, true},
    {GL_R32F, GL_RED, GL_FLOAT, BasicType::FLOAT, 32, 1, false, false, true},
    {GL_RG32F, GL_RG, GL_FLOAT, BasicType::FLOAT, 64, 2, false, false, true},
    {GL_RGBA32F, GL_RGBA, GL_FLOAT, BasicType::FLOAT, 128, 4, true, false, true},
    {GL_R8, GL_RED, GL_UNSIGNED_BYTE, BasicType::UINT8, 8, 1},
    {GL_RG8, GL_RG, GL_UNSIGNED_BYTE, BasicType::UINT8, 16, 2},
    {GL_R16, GL_RED, GL_UNSIGNED_SHORT, BasicType::UINT16, 16, 1},
    {GL_RG16, GL_RG, GL_UNSIGNED_SHORT, BasicType::UINT16, 32, 2},
    {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, BasicType::UINT16, 16, 1, false, true},
    {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, BasicType::UINT32, 32, 1, false, true}, // WARNING: may not be renderable (?!)
    {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, BasicType::UINT32, 32, 1, false, true},
    {0, 0, 0, BasicType::UINT8, 8, 1, true, false, false, true}
}};

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(PixelFormat format, u32 width, u32 height)
{
	return pixelFormatInfo[format].size / 8 * width * height;
}

std::string formatToStr(PixelFormat format)
{
    switch(format) {
    case PF_RGB8:
        return "RGB8";
    case PF_RGBA8:
        return "RGBA8";
    case PF_R16F:
        return "R16F";
    case PF_RG16F:
        return "RG16F";
    case PF_RGBA16F:
        return "RGBA16F";
    case PF_R32F:
        return "R32F";
    case PF_RG32F:
        return "RG32F";
    case PF_RGBA32F:
        return "RGBA32F";
    case PF_R8:
        return "R8";
    case PF_RG8:
        return "RG8";
    case PF_R16:
        return "R16";
    case PF_RG16:
        return "RG16";
    case PF_D16:
        return "D16";
    case PF_D32:
        return "D32";
    case PF_D24S8:
        return "D24S8";
    case PF_INDEX_RGBA8:
        return "INDEX_RGBA8";
    default:
        return "COUNT";
    }
}

// Functions extracting one of the channels of the RGB8 or RGBA8 formats.
u32 getRedMask(PixelFormat format)
{
	switch (format) {
    case PF_R8:
    case PF_INDEX_RGBA8:
        return 0xFF;
    case PF_RG8:
        return 0xFF00;
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
    case PF_R8:
    case PF_INDEX_RGBA8:
        return 0x0;
    case PF_RG8:
        return 0x00FF;
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
    case PF_R8:
    case PF_INDEX_RGBA8:
    case PF_RG8:
        return 0x0;
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
    case PF_R8:
    case PF_INDEX_RGBA8:
    case PF_RG8:
    case PF_RGB8:
        return 0x0;
    case PF_RGBA8:
        return 0x000000FF;
    default:
        return 0x0;
	}
}

}
