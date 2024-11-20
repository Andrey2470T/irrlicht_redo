#pragma once

#include "Image.h"
#include "Utils/Rect.h"

namespace img
{

struct BlendMode
{
	bool Enabled = false;

	render::BlendFunc SrcFunc;
	render::BlendFunc DstFunc;

	render::BlendOp Op = render::BO_ADD;
};

//! Does blitting without/with scaling and filling with some color in the set blend mode.
class ImageModifier
{
	BlendMode mode;
public:
	ImageModifier() {}

	// Returns a pixel from the image
	color8 getPixel(const Image *const img, u32 x, u32 y) const;

	//! Sets a pixel for the image
	void setPixel(
		const Image *img,
		u32 x, u32 y,
		const color8 &color);

	// Fills the whole (or part) of the image with the given color
	// Absence of the passed rect means filling the whole image
	void fill(
		const Image *img,
		const color8 &color,
		const Rect<u32> *rect = nullptr);

	// Copies the whole (or part) of the source image to the whole (or part) of the dest image
	void copyTo(
		const Image *const srcImg,
		const Image *dstImg,
		const utils::recti *const srcRect = nullptr,
		const utils::recti *const dstRect = nullptr,
		bool allowScale = false);

	// Scales (up or down) the image with the given factor
	void scale(const Image *img, utils::v2i scaleFactor);

	// Rotates the given image (clockwise or counter-clockwise) by the given angle
	void rotate(const Image *img, s32 angle);

	bool blendEnabled() const
	{
		return mode.Enabled;
	}

	void toggleBlend()
	{
		mode.Enabled = !mode.Enabled;
	}

	void setBlendMode(render::BlendFunc srcF, render::BlendFunc dstF, render::BlendOp op = render::BO_ADD)
	{
		mode.SrcFunc = srcF;
		mode.DstFunc = dstF;
		mode.Op = op;
	}
};

}
