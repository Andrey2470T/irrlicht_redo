#include "Image.h"
#include "ImageModifier.h"
#include <algorithm>

namespace img
{

Image::Image(PixelFormat _format, u32 _width, u32 _height, color8 _initColor,
    Palette *_palette, ImageModifier *mdf)
	: format(_format), width(_width), height(_height)
{
	if (!isFormatSupportedForImage(format)) {
		SDL_LogError(LC_VIDEO, "Image::Image() unsupported format for Image");
		return;
	}
	u32 pixelSize = pixelFormatInfo.at(format).size / 8;

	data = new u8[width * height * pixelSize];

	if (_palette)
		palette = std::make_unique<Palette>(_palette->hasAlpha, _palette->size, _palette->colors);
	else
		palette = std::make_unique<Palette>(false, 0, 0);

    if (mdf) {
        mdf->fill(this, _initColor);
    }

	ownPixelData = true;
}

Image::Image(PixelFormat _format, u32 _width, u32 _height, u8 *_data,
	bool _copyData, Palette *_palette)
	: format(_format), width(_width), height(_height)
{
	if (!isFormatSupportedForImage(format)) {
		SDL_LogError(LC_VIDEO, "Image::Image() unsupported format for Image");
		return;
	}
	ownPixelData = _copyData;

	if (_palette)
		palette = std::make_unique<Palette>(_palette->hasAlpha, _palette->size, _palette->colors);
	else
		palette = std::make_unique<Palette>(false, 0, 0);

	if (!ownPixelData)
		data = _data;
	else {
		u32 pixelSize = pixelFormatInfo.at(format).size / 8;

		data = new u8[width * height * pixelSize];
        memcpy(data, _data, width * height * pixelSize);
	}
}

Image::~Image()
{
	if (ownPixelData)
		delete[] data;
}

void Image::setPaletteColors(const std::vector<color8> &colors)
{
	palette->colors.clear();
	palette->size = colors.size();
	palette->hasAlpha = pixelFormatInfo.at(colors.at(0)->getFormat()).hasAlpha;
    u8 size = std::min<u32>(255u, colors.size());
	palette->colors.reserve(size);
		
	for (u8 i = 0; i < size; i++)
		palette->colors.push_back(colors[i]);
}

Image *Image::copy() const
{
	Image *newImg = new Image(getFormat(), getWidth(), getHeight(), getData(), true);
	return newImg;
}

}
