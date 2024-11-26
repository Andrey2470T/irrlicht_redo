#pragma once

#include "Image.h"
#include "Utils/Rect.h"

namespace img
{

enum RESAMPLE_FILTER
{
	RF_NEAREST,
	RF_BILINEAR,
	RF_BICUBIC,
	RF_LANCZOS
};

enum ROTATE_ANGLE
{
	RA_90,
	RA_180,
	RA_270
};

enum FLIP_DIR
{
	FD_X,
	FD_Y
};

enum BLEND_MODE
{
	BM_NORMAL = 0,
	BM_ALPHA,
	BM_ADD,
	BM_SUBTRACTION,
	BM_MULTIPLY,
	BM_DIVISION,
	BM_SCREEN,
	BM_OVERLAY,
	BM_HARD_LIGHT,
	BM_SOFT_LIGHT,
	BM_GRAIN_EXTRACT,
	BM_GRAIN_MERGE,
	BM_DIFFERENCE,
	BM_DARKEN_ONLY,
	BM_LIGHTEN_ONLY,
	BM_TONE,
	BM_COUNT
};

static std::array<std::function<>, BM_COUNT>
struct BlendMode
{
	bool Enabled = false;

	render::BlendFunc SrcFunc;
	render::BlendFunc DstFunc;

	render::BlendOp Op = render::BO_ADD;

	bool isAlphaBlending()
	{
		bool is_yes = false;
		switch (SrcFunc) {
			case render::BF_SRC_ALPHA:
			case render::BF_ONE_MINUS_DST_ALPHA:
				is_yes = true;
				break;
			default:
				break;
		};

		switch (DstFunc) {
			case render::BF_ONE_MINUS_SRC_ALPHA:
			case render::BF_DST_ALPHA:
				is_yes = true;
				break;
			default:
				break;
		};

		return is_yes;
	}
};

//! Does blitting without/with scaling and filling with some color in the set blend mode.
class ImageModifier
{
	BlendMode Mode;
public:
	ImageModifier() {}

	// Returns a pixel from the image
	color8 getPixel(const Image *img, u32 x, u32 y) const;

	//! Sets a pixel for the image
	void setPixel(
		Image *img,
		u32 x, u32 y,
		const color8 &color);

	// Fills the whole (or part) of the image with the given color
	// Absence of the passed rect means filling the whole image
	void fill(
		Image *img,
		const color8 &color,
		const utils::recti *rect = nullptr);

	// Copies the whole (or part) of the source image to the whole (or part) of the dest image
	/// @param "srcImg" - image whose the pixel data is copied
	/// @param "dstImg" - image where the copy happens to
	/// @param "srcRect" - part of the image which will be copied
	/// @param "dstRect" - part of the image where the copy will be done to
	/// @param "allowScale" - if true, the copy will occur with scaling before the "dstRect" bounds (upscaling or downscaling)
	void copyTo(
		const Image * srcImg,
		Image *dstImg,
		const utils::recti *srcRect = nullptr,
		const utils::recti *dstRect = nullptr,
		bool allowScale = false);

	// Scales (up or down) the image with the given factor.
	// The convolution algorithm is used with one of filter types.
	void scale(Image *img, utils::v2i scaleFactor, RESAMPLE_FILTER filter=RF_NEAREST);

	// Rotates the given image by the angle multiple by 90 degrees
	void rotate(Image *img, ROTATE_ANGLE angle);
	
	void flip(Image *img, FLIP_DIR dir);
	
	Image *crop(const Image *img, const utils::recti &rect);
	
	Image *createNew(u32 width, u32 height, const color8 &color=color8(255,255,255,255));
	
	Image *combine(const Image *img1, const Image *img2);

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
