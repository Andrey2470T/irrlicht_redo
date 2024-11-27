#include "ImageModifier.h"

namespace img
{

// Returns a pixel from the image
color8 ImageModifier::getPixel(const Image *img, u32 x, u32 y) const
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height)
		SDL_LogWarn(LC_VIDEO, "ImageModifier::getPixel() coordinates are out of image size");
		return color8(PF_RGB8);

	u8 *data = img->getData();

	switch(img->getFormat()) {
		case PF_RGB8:
			u8 *pixel = data[y * 3 * width + 3 * x];
			return color8(PF_RGB8, *pixel, *pixel++, *pixel++);
		case PF_RGBA8:
			u8 *pixel = data[y * 4 * width + 4 * x];
			return color8(PF_RGBA8, *pixel, *pixel++, *pixel++, *pixel++);
		case PF_COUNT:
			SDL_LogWarn(LC_VIDEO, "ImageModifier::getPixel() unknown format");
			return color8(PF_RGB8);
			break;
		default:
			return color8(PF_RGB8);
	}
}

//! Sets a pixel for the image
void ImageModifier::setPixel(
	Image *img,
	u32 x, u32 y,
	const color8 &color)
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height)
		SDL_LogWarn(LC_VIDEO, "ImageModifier::setPixel() coordinates are out of image size");
		return;

	u8 *data = img->getData();

	u8 *pixel = nullptr;

	PixelFormat format = img->getFormat();
	if (format == PF_RGB8)
		pixel = data[y * 3 * width + 3 * x];
	else if (format == PF_RGBA8)
		pixel = data[y * 4 * width + 4 * x];
	else {
		SDL_LogWarn(LC_VIDEO, "ImageModifier::setPixel() unknown format");
		return;
	}

	color8 new_c(format, *pixel, *(pixel)+1, *(pixel)+2, 0);
	new_c = doBlend<u8>(color, new_c, Mode);

	*pixel = new_c.R();
	*(pixel+1) = new_c.G();
	*(pixel+2) = new_c.B();
}

// Fills the whole (or part) of the image with the given color
// Absence of the passed rect means filling the whole image
void ImageModifier::fill(
	Image *img,
	const color8 &color,
	const utils::recti *rect)
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (!rect) {
		for (u32 x = 0; x < width; x++)
			for (u32 y = 0; y < height; y++)
				setPixel(img, x, y, color);
	}
	else {
		utils::recti img_rect(utils::v2i((s32)width, (s32)height));
		if (!img_rect.isRectInside(*rect)) {
			SDL_LogWarn(LC_VIDEO, "ImageModifier::fill() sub rect is outside of image size");
			return;
		}

		for (u32 x = rect->ULC.X; x < rect->LRC.X; x++)
			for (u32 y = rect->ULC.Y; y < rect->LRC.Y; y++)
				setPixel(img, x, y, color);
	}
}
}
