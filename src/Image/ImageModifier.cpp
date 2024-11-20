#include "ImageModifier.h"

namespace img
{

static const color8 void blendPixels(
	const color8 &src,
	const color8 &dst,
	const BlendMode &mode)
{
	if (!mode.Enabled)
		// Means just overlay one color atop other one
		return src;

	color8 c1(src);
	color8 c2(dst);

	switch (mode.SrcFunc) {
		case render::BF_ZERO:
			c1 *= 0;
			break;
		case render::BF_ONE_MINUS_DST_COLOR:
		{
			c1.R = 255 - c2.R;
			c1.G = 255 - c2.G;
			c1.B = 255 - c2.B;
			break;
		}
		case render::BF_SRC_ALPHA:
			c1 *= c1.A;
			break;
		case render::BF_ONE_MINUS_DST_ALPHA:
			c1 *= (255 - c2.A);
			break;
		default:
			break;
	};

	switch (mode.DstFunc) {
		case render::BF_ZERO:
			c2 *= 0;
			break;
		case render::BF_ONE_MINUS_SRC_COLOR:
		{
			c2.R = 255 - c1.R;
			c2.G = 255 - c1.G;
			c2.B = 255 - c1.B;
			break;
		}
		case render::BF_DST_ALPHA:
			c2 *= c2.A;
			break;
		case render::BF_ONE_MINUS_SRC_ALPHA:
			c2 *= (255 - c1.A);
			break;
		default:
			break;
	};

	switch (mode.Op) {
		case render::BO_ADD:
			return c1 + c2;
		case render::BO_SUBTRACT:
			return c1 - c2;
		case render::BO_MULTIPLY:
			return c1 * c2;
		default:
			return c1 + c2;
	}
}

// Returns a pixel from the image
color8 ImageModifier::getPixel(const Image *img, u32 x, u32 y) const
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height)
		SDL_LogWarn(LC_VIDEO, "ImageModifier::getPixel() coordinates are out of image size");
		return color8(0);

	u8 *data = img->getData();

	switch(img->getFormat()) {
		case PF_RGB8:
			u8 *pixel = data[y * 3 * width + 3 * x];
			return color8(*pixel, *pixel++, *pixel++);
		case PF_RGBA8:
			u8 *pixel = data[y * 4 * width + 4 * x];
			return color8(*pixel, *pixel++, *pixel++, *pixel++);
		case PF_COUNT:
			SDL_LogWarn(LC_VIDEO, "ImageModifier::getPixel() unknown format");
			return color8(0);
			break;
		default:
			return color8(0);
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

	if (img->getFormat() == PF_RGB8 && !Mode.isAlphaBlending()) // Alpha blending can not be with RGB format (just no alpha for that)
		pixel = data[y * 3 * width + 3 * x];
	else if (img->getFormat() == PF_RGBA8)
		pixel = data[y * 4 * width + 4 * x];
	else {
		SDL_LogWarn(LC_VIDEO, "ImageModifier::setPixel() unknown format");
		return;
	}

	color8 new_c(*pixel, *(pixel)+1, *(pixel)+2, 0);
	new_c = blendPixels(color, new_c, Mode);

	*pixel = new_c.R;
	*(pixel+1) = new_c.G;
	*(pixel+2) = new_c.B;
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
