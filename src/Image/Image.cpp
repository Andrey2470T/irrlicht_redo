#include "Image.h"

namespace img
{

template<T>
ColorRGBA<T> pixelAlphaBlend(const ColorRGBA<T> &c1, const ColorRGBA<T> &c2)
{
	ColorRGBA<T> newColor;
	newColor.R = lerp<T>(c2.R, c1.R, c1.A);
	newColor.G = lerp<T>(c2.G, c1.G, c1.A);
	newColor.B = lerp<T>(c2.B, c1.B, c1.A);
	newColor.A = c1.A;
	
	return newColor;
}

Image::Image(ImageFormat _format, u32 _width, u32 _height)
	: format(_format), width(_width), height(_height)
{
	u32 pixelSize = toGLFormatConverter.at(format).size / 8;
	
	data = new u8[width * height * pixelSize];
	
	ownPixelData = true;
}

Image::Image(ImageFormat _format, u32 _width, u32 _height, u8 *_data, bool copyData)
	: format(_format), width(_width), height(_height)
{
	ownPixelData = copyData;
	
	if (!ownPixelData)
		data = _data;
	else {
		u32 pixelSize = toGLFormatConverter.at(format).size / 8;
	
		data = new u8[width * height * pixelSize];
		std::memcpy(data, _data, width * height * pixelSize);
	}
}

Image::~Image()
{
	if (ownPixelData)
		delete[] data;
}

//! Returns a pixel
ColorRGBA<u8> Image::getPixel(u32 x, u32 y) const
{
	if (x >= width || y >= height)
		return ColorRGBA<u8>(0);
	
	switch(format) {
		case IF_RGB8:
			u8 *pixel = data[y * 3 * width + 3 * x];
			return ColorRGBA<u8>(*pixel, *pixel++, *pixel++);
		case IF_RGBA8:
			u8 *pixel = data[y * 4 * width + 4 * x];
			return ColorRGBA<u8>(*pixel, *pixel++, *pixel++, *pixel++);
		case IF_COUNT:
			os::Printer::log("Image::getPixel unknown format.", ELL_WARNING);
			return ColorRGBA<u8>(0);
			break;
		default:
			return ColorRGBA<u8>(0);
	}
}

//! Sets a pixel
void Image::setPixel(u32 x, u32 y, const ColorRGBA<u8> &color, bool blend)
{
	if (x >= width || y >= height)
		return;
	
	switch(format) {
		case IF_RGB8: {
			u8 *pixel = data[y * 3 * width + 3 * x];
			pixel = color.R;
			*pixel++ = color.G;
			*pixel++ = color.B;
			break;
		}
		case IF_RGBA8: {
			u8 *pixel = data[y * 4 * width + 4 * x];
			
			if (blend) {
				auto res = pixelAlphaBlend(color, ColorRGBA<u8>(*pixel, *(pixel+1), *(pixel+2), *(pixel+3)));
				
				std::memcpy(pixel, &res, 3);
			}
			else {
				std::memcpy(pixel, &color, 3);
			}
			break;
		}
		case IF_COUNT:
			os::Printer::log("Image::getPixel unknown format.", ELL_WARNING);
			break;
		default:
			break;
	}
}

//! fills the whole image or a subimage clipped by 'rect' with given color
void Image::fill(const ColorRGBA<u8> &color, const Rect<u32> *rect = nullptr)
{
	Rect<u32> imgArea(width, height);
	
	if (!imgArea.isRectInside(*rect))
		return;
	
	for (u32 x = rect->ULC.X; x <= rect->LRC.X; x++)
		for (u32 y = rect->ULC.Y; y <= rect->LRC.Y; y++)
			setPixel(x, y, color);
}

}
