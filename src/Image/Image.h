#pragma once

#include "Render/Common.h"
#include "PixelFormats.h"
#include "Color.h"

namespace img {

class ImageModifier;

bool isFormatSupportedForImage(PixelFormat format);

/* 
 * 8-bit palette (can be with alpha or without).
 * Indices are u8.
 */
struct Palette
{
	bool hasAlpha = false;
	u32 size;
	
	std::vector<color8> colors;
	
	Palette(bool _hasAlpha, u32 _size)
		: hasAlpha(_hasAlpha), size(_size)
	{
		colors.resize(size);
	}
	Palette(bool _hasAlpha, u32 _size, const std::vector<color8> &_colors)
		: hasAlpha(_hasAlpha), size(_size), colors(_colors)
	{}

	u8 findColorIndex(const color8 &c);
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
    Image(PixelFormat _format, u32 _width, u32 _height, color8 _initColor=color8(PF_RGB8, 0, 0, 0),
          Palette *_palette = nullptr, ImageModifier *mdf = nullptr);

	Image(PixelFormat _format, u32 _width, u32 _height, u8 *_data,
		  bool _copyData = true, Palette *palette = nullptr);

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
        return data;
	}

	void setPaletteColors(const std::vector<color8> &colors);
	
	Image *copy() const;
};

}
