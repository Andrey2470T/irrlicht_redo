#pragma once

#include "Render/Common.h"
#include "PixelFormats.h"
#include "Color.h"

namespace img {

/* 
 * 8-bit palette (can be with alpha or without)
 */
struct Palette
{
	bool hasAlpha;
	u32 size;
	
	std::vector<color8> colors;
};

/* Image representation class referring or saving directly
 * the pixel data. Can be RGB, RGBA or indexed to one of these types integral 8-bit format.
 */
class Image
{
	PixelFormat format;

	u32 width;
	u32 height;

	u8* data;
	
	std::unique_ptr<Palette> palette;

	bool ownPixelData;
public:
	Image(PixelFormat _format, u32 _width, u32 _height, color8 initColor=color8(PF_RGB, 0, 0, 0));

	Image(PixelFormat _format, u32 _width, u32 _height, u8 *data,
		  bool copyData = true, color8 initColor=color8(PF_RGB, 0, 0, 0));

	~Image();

	PixelFormat getFormat() const
	{
		return format;
	}

	u32 getWidth() const
	{
		return width;
	}

	u32 getHeight() const
	{
		return height;
	}

	utils::v2u getSize() const
	{
		return utils::v2u(width, height);
	}

	Palette *getPalette() const
	{
		return palette.get();
	}

	u8* getData() const
	{
		return data.get();
	}
	
	Image *copy() const;
};

}
