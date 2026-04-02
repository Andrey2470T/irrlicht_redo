#pragma once

#include "irrTypes.h"
#include <array>
#include <string>

namespace video
{
//! An enum for the color format of textures used by the Irrlicht Engine.
/** A color format specifies how color information is stored.
	NOTE: Byte order in memory is usually flipped (it's probably correct in bitmap files, but flipped on reading).
	So for example ECF_A8R8G8B8 is BGRA in memory same as in DX9's D3DFMT_A8R8G8B8 format.
*/
enum ECOLOR_FORMAT
{
	//! 16 bit color format used by the software driver.
	/** It is thus preferred by all other irrlicht engine video drivers.
	There are 5 bits for every color component, and a single bit is left
	for alpha information. */
	ECF_A1R5G5B5 = 0,

	//! Standard 16 bit color format.
	ECF_R5G6B5,

	//! 24 bit color, no alpha channel, but 8 bit for red, green and blue.
	//! Warning: 24 bit formats tend to be badly supported. Depending on driver it's usually converted to another
	//           format or even not working at all. It's mostly better to use 16-bit or 32-bit formats.
	ECF_R8G8B8,

	//! Default 32 bit color format. 8 bits are used for every component: red, green, blue and alpha.
	//! Warning: This tends to be BGRA in memory (it's ARGB on file, but with usual big-endian memory it's flipped)
	ECF_A8R8G8B8,

	/** The following formats may only be used for render target textures. */

	/** Floating point formats. */

	//! 16 bit format using 16 bits for the red channel.
	ECF_R16F,

	//! 32 bit format using 16 bits for the red and green channels.
	ECF_G16R16F,

	//! 64 bit format using 16 bits for the red, green, blue and alpha channels.
	ECF_A16B16G16R16F,

	//! 32 bit format using 32 bits for the red channel.
	ECF_R32F,

	//! 64 bit format using 32 bits for the red and green channels.
	ECF_G32R32F,

	//! 128 bit format using 32 bits for the red, green, blue and alpha channels.
	ECF_A32B32G32R32F,

	/** Unsigned normalized integer formats. */

	//! 8 bit format using 8 bits for the red channel.
	ECF_R8,

	//! 16 bit format using 8 bits for the red and green channels.
	ECF_R8G8,

	//! 16 bit format using 16 bits for the red channel.
	ECF_R16,

	//! 32 bit format using 16 bits for the red and green channels.
	ECF_R16G16,

	//! 32 bit format using 10 bits for R, G, B and 2 for alpha.
	ECF_A2R10G10B10,

	/** Depth and stencil formats. */

	//! 16 bit format using 16 bits for depth.
	ECF_D16,

	//! 32 bit(?) format using 24 bits for depth.
	ECF_D24,

	//! 32 bit format using 32 bits for depth.
	ECF_D32,

	//! 32 bit format using 24 bits for depth and 8 bits for stencil.
	ECF_D24S8,

	//! Unknown color format:
	ECF_UNKNOWN
};

struct PixelFormatInfo
{
	std::string name;
	u32 size; 			// in bits

	bool hasAlpha = false;
	bool isDepth = false;
	bool isFloatingPoint = false;
};

extern const std::array<PixelFormatInfo, ECF_UNKNOWN+1> pixelFormatsInfo;

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(ECOLOR_FORMAT format, u32 width, u32 height);

//! Returns mask for red value of a pixel
u32 getRedMask(ECOLOR_FORMAT format);
//! Returns mask for green value of a pixel
u32 getGreenMask(ECOLOR_FORMAT format);
//! Returns mask for blue value of a pixel
u32 getBlueMask(ECOLOR_FORMAT format);
//! Returns mask for alpha value of a pixel
u32 getAlphaMask(ECOLOR_FORMAT format);

}  // end namespace video
