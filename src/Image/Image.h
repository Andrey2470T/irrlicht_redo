#pragma once

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

    img::color8 getColorByIndex(u32 index) const
    {
        if (index >= size)
            return img::color8(img::PF_RGBA8, 0, 0, 0, 0);
        return colors.at(index);
    }

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

    // Defines an active subimage area
    struct ClipRegion {
        v2u pos;
        v2u size;
    } clipregion;
public:
    Image(PixelFormat _format, u32 _width, u32 _height, const color8 &_initColor=img::black,
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

    v2u getSize() const
	{
        return v2u(width, height);
	}

    v2u getClipPos() const
    {
        return clipregion.pos;
    }

    v2u getClipSize() const
    {
        return clipregion.size;
    }

	Palette *getPalette() const
	{
		return palette.get();
	}

    u8* getData() const;

	void setPaletteColors(const std::vector<color8> &colors);

    void setClipRegion(u32 x, u32 y, u32 size_x, u32 size_y);
	
	Image *copy() const;
	
	bool operator==(const Image *other)
	{
		u32 pixelSize = pixelFormatInfo.at(format).size / 8;
		return (format == other->format && width == other->getWidth() && height == other->getHeight() &&
		    !memcmp(data, other->getData(), width * height * pixelSize));
    }
};

}
