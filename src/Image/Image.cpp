#include "Image.h"
#include "Image/Converting.h"
#include "ImageModifier.h"

namespace img
{

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

Image::Image(PixelFormat _format, u32 _width, u32 _height, const color8 &_initColor,
    Palette *_palette, ImageModifier *mdf, std::optional<u32> _formatsEnumsIndex)
	: format(_format), width(_width), height(_height)
{
	if (!isFormatSupportedForColor8(format)) {
		ErrorStream << "Image::Image() unsupported format for Image\n";
		return;
	}
	u32 pixelSize = pixelFormatInfo.at(format).size / 8;

	data = new u8[width * height * pixelSize];

    if (format == PF_INDEX_RGBA8) {
	    if (_palette)
            palette = std::make_unique<Palette>(true, _palette->size, _palette->colors);
	    else
            palette = std::make_unique<Palette>(true, 0);
	}

    if (!_formatsEnumsIndex.has_value()) {
        auto it = std::find_if(formatsEnumsMap.begin(), formatsEnumsMap.end(),
            [&] (const std::pair<u32, u32> &p)
            {
                return p.second == format;
            });
        formatsEnumsIndex = it->first;
    }
    else
        formatsEnumsIndex = _formatsEnumsIndex.value();

    pitch = pixelSize * width;

    if (mdf) {
        mdf->fill(this, _initColor);
    }

	ownPixelData = true;

    clipregion.pos = v2u(0, 0);
    clipregion.size = v2u(width, height);
}

Image::Image(PixelFormat _format, u32 _width, u32 _height, u8 *_data,
    bool _copyData, Palette *_palette, std::optional<u32> _pitch,
    std::optional<u32> _formatsEnumsIndex)
	: format(_format), width(_width), height(_height)
{
	if (!isFormatSupportedForColor8(format)) {
		ErrorStream << "Image::Image() unsupported format for Image\n";
		return;
	}
	ownPixelData = _copyData;

    if (format == PF_INDEX_RGBA8) {
	    if (_palette)
            palette = std::make_unique<Palette>(true, _palette->size, _palette->colors);
	    else
            palette = std::make_unique<Palette>(true, 0);
	}

    if (!_formatsEnumsIndex.has_value()) {
        auto it = std::find_if(formatsEnumsMap.begin(), formatsEnumsMap.end(),
            [&] (const std::pair<u32, u32> &p)
            {
                return p.second == format;
            });
        formatsEnumsIndex = it->first;
    }
    else
        formatsEnumsIndex = _formatsEnumsIndex.value();

    u32 pixelSize = pixelFormatInfo.at(format).size / 8;
	if (!ownPixelData)
		data = _data;
	else {
		data = new u8[width * height * pixelSize];
        memcpy(data, _data, width * height * pixelSize);
	}

    if (!_pitch.has_value())
        pitch = pixelSize * width;
    else
        pitch = _pitch.value();

    clipregion.pos = v2u(0, 0);
    clipregion.size = v2u(width, height);
}

Image::~Image()
{
	if (ownPixelData)
		delete[] data;
}

u8* Image::getData() const
{
	u32 pixelSize = pixelFormatInfo.at(format).size / 8;
    return data + clipregion.pos.Y * pitch + clipregion.pos.X * pixelSize;
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

void Image::setClipRegion(u32 x, u32 y, u32 size_x, u32 size_y)
{
    clipregion.pos.X = x;
    clipregion.pos.Y = y;
    clipregion.size.X = size_x;
    clipregion.size.Y = size_y;
}

Image *Image::copy() const
{
    v2u clipSize = getClipSize();
    Image *newImg = new Image(
        getFormat(), clipSize.X, clipSize.Y, getData(), true, getPalette(), getPitch(), getFormatsEnumsIndex());
	return newImg;
}

}
