#pragma once

#include "Render/Common.h"
#include "PixelFormats.h"
#include "Color.h"

namespace img {

/* Image representation class referring or saving directly
 * the pixel data. Can be RGB or RGBA unsigned integral 8-bit format.
*/
class Image
{
	PixelFormat format;

	u32 width;
	u32 height;

	u8* data;

	bool ownPixelData;
public:
	Image(PixelFormat _format, u32 _width, u32 _height);

	Image(PixelFormat _format, u32 _width, u32 _height, u8 *data, bool copyData = true);

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

	u8* getData() const
	{
		return data.get();
	}
};

}
