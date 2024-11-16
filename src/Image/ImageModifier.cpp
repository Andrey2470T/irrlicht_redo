#include "ImageModifier.h"

namespace img
{

const ColorRGBA<u8> void blendPixels(
	const ColorRGBA<u8> &src,
	const ColorRGBA<u8> &dst,
	const BlendMode &mode)
{
	if (!mode.enabled)
		return dst;

	s8 op_sign = 1;
	switch (mode.eq) {
		case render::BO_ADD:
			op_sign = 1;
			break;
		case render::BO_SUBTRACT:
			op_sign = -1;
			break;
		default:
			break;
	};

	ColorRGBA<u8> c1(src);
	ColorRGBA<u8> c2(dst);

	switch (mode.srcFunc) {
		case render::BF_ZERO:
			c1 *= 0;
			break;
		case render::BF_ONE_MINUS_SRC_COLOR:
		{
			c1.R = 1 - c1.R;
			c1.G = 1 - c1.G;
			c1.B = 1 - c1.B;
			break;
		}
		case render::BF_SRC_ALPHA:
			c1 *= c1.A;
			break;
		case render::BF_ONE_MINUS_SRC_ALPHA:
			c1 *= (1 - c1.A);
			break;
		default:
			break;
	};

	switch (mode.dstFunc) {
		case render::BF_ZERO:
			c2 *= 0;
			break;
		case render::BF_ONE_MINUS_DST_COLOR:
		{
			c2.R = 1 - c2.R;
			c2.G = 1 - c2.G;
			c2.B = 1 - c2.B;
			break;
		}
		case render::BF_DST_ALPHA:
			c2 *= c2.A;
			break;
		case render::BF_ONE_MINUS_DST_ALPHA:
			c2 *= (1 - c2.A);
			break;
		default:
			break;
	};

	return c1 + c2 * op_sign;
}

// Returns a pixel from the image
ColorRGBA<u8> ImageModifier::getPixel(const Image *const img, u32 x, u32 y) const
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height)
		return ColorRGBA<u8>(0);

	switch(img->getFormat()) {
		case PF_RGB8:
			u8 *pixel = data[y * 3 * width + 3 * x];
			return ColorRGBA<u8>(*pixel, *pixel++, *pixel++);
		case PF_RGBA8:
			u8 *pixel = data[y * 4 * width + 4 * x];
			return ColorRGBA<u8>(*pixel, *pixel++, *pixel++, *pixel++);
		case PF_COUNT:
			SDL_LogWarn(LC_VIDEO, "Image::getPixel() unknown format");
			return ColorRGBA<u8>(0);
			break;
		default:
			return ColorRGBA<u8>(0);
	}
}

//! Sets a pixel for the image
void ImageModifier::setPixel(
	const Image *img,
	u32 x, u32 y,
	const ColorRGBA<u8> &color,
	const BlendMode &mode)
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height)
		return;

	switch(img->getFormat()) {
		case PF_RGB8: {
			u8 *pixel = data[y * 3 * width + 3 * x];
			pixel = color.R;
			*pixel++ = color.G;
			*pixel++ = color.B;
			break;
		}
		case PF_RGBA8: {
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
		case PF_COUNT:
			SDL_LogWarn(LC_VIDEO, _"Image::getPixel() unknown format");
			break;
		default:
			break;
	}
}
}
