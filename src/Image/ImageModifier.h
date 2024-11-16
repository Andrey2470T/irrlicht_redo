#pragma once

#include "Image.h"
#include "Utils/Rect.h"

namespace img
{

struct BlendMode
{
	bool enabled = false;

	render::BlendFunc srcFunc;
	render::BlendFunc dstFunc;

	render::BlendOp eq = render::BO_ADD;
};

//! Does blitting without/with scaling and filling with some color in the set blend mode.
class ImageModifier
{
public:
	ImageBlitter() = delete;

	// Returns a pixel from the image
	static ColorRGBA<u8> getPixel(const Image *const img, u32 x, u32 y) const;

	//! Sets a pixel for the image
	static void setPixel(
		const Image *img,
		u32 x, u32 y,
		const ColorRGBA<u8> &color,
		const BlendMode &mode = BlendMode());

	// Fills the whole (or part) of the image with the given color
	// Absence of the passed rect means filling the whole image
	static void fill(
		const Image *img,
		const ColorRGBA<u8> &color,
		const Rect<u32> *rect = nullptr,
		const BlendMode &mode = BlendMode());

	// Copies the whole (or part) of the source image to the whole (or part) of the dest image
	static void copyTo(
		const Image *const srcImg,
		const Image *dstImg,
		const utils::recti *const srcRect = nullptr,
		const utils::recti *const dstRect = nullptr,
		const BlendMode &mode = BlendMode(),
		bool allowScale = false);

	// Scales (up or down) the image with the given factor
	static void scale(const Image *img, utils::v2i scaleFactor);

	// Rotates the given image (clockwise or counter-clockwise) by the given angle
	static void rotate(const Image *img, s32 angle);
};

}
