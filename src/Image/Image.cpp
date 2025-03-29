#include "Image.h"
#include "ImageModifier.h"

namespace img
{

bool isFormatSupportedForImage(PixelFormat format)
{
	switch (format) {
		case PF_RGB8:
		case PF_RGBA8:
		case PF_INDEX_RGBA8:
			return true;
		default:
			return false;
	}
}

u8 Palette::findColorIndex(const color8 &c)
{
	color8 minColorDiff(c.getFormat());
	u32 index = 0;

	for (u32 i = 0; i < size; i++) {
		color8 &cur_c = colors.at(i);
		color8 colorDiff(c.getFormat(),
			(u8)std::abs((s16)c.R()-(s16)cur_c.R()),
			(u8)std::abs((s16)c.G()-(s16)cur_c.G()),
			(u8)std::abs((s16)c.B()-(s16)cur_c.B()),
			(u8)std::abs((s16)c.A()-(s16)cur_c.A()));

		if (colorDiff < minColorDiff) {
			minColorDiff = colorDiff;
			index = i;
		}
	}
		
	return index;
}

Image::Image(PixelFormat _format, u32 _width, u32 _height, color8 _initColor,
    Palette *_palette, ImageModifier *mdf)
	: format(_format), width(_width), height(_height)
{
	if (!isFormatSupportedForImage(format)) {
		ErrorStream << "Image::Image() unsupported format for Image\n";
		return;
	}
	u32 pixelSize = pixelFormatInfo.at(format).size / 8;

	data = new u8[width * height * pixelSize];

	if (_palette)
		palette = std::make_unique<Palette>(_palette->hasAlpha, _palette->size, _palette->colors);
	else
		palette = std::make_unique<Palette>(false, 0);

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
		ErrorStream << "Image::Image() unsupported format for Image\n";
		return;
	}
	ownPixelData = _copyData;

	if (_palette)
		palette = std::make_unique<Palette>(_palette->hasAlpha, _palette->size, _palette->colors);
	else
		palette = std::make_unique<Palette>(false, 0);

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
    palette->hasAlpha = pixelFormatInfo.at(colors.at(0).getFormat()).hasAlpha;
    u8 size = std::min<u32>(255u, colors.size());
	palette->colors.resize(size);

	for (u8 i = 0; i < size; i++)
		palette->colors.at(i) = colors[i];
}

Image *Image::copy() const
{
	Image *newImg = new Image(getFormat(), getWidth(), getHeight(), getData(), true);
	return newImg;
}

}
