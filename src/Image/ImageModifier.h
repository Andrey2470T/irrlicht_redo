#pragma once

#include "Image.h"
#include "BlendModes.h"
#include "Utils/Rect.h"
#include "ResizeFilters.h"

namespace img
{

enum ROTATE_ANGLE : u8
{
	RA_90,
	RA_180,
	RA_270
};

enum FLIP_DIR : u8
{
	FD_X,
	FD_Y
};

//! Does blitting without/with scaling and filling with some color in the set blend mode.
class ImageModifier
{
    BlendMode Mode;
	bool BlendEnabled;
public:
	ImageModifier() {}

	// Returns a pixel from the image
	color8 getPixel(const Image *img, u32 x, u32 y) const;

	// Sets a pixel for the image
	void setPixel(Image *img, u32 x, u32 y, const color8 &color);

	// Return (only) a pixel color from the image
	// If the format is indexed, it will extract the color from the palette, not its index
	color8 getPixelColor(const Image *img, u32 x, u32 y) const;

	// Sets (only) a pixel color for the image
	// If the format is indexed, it will find the closest pixel color from the palette, so "color" shoudln't be indexed
	void setPixelColor(Image *img, u32 x, u32 y, const color8 &color);

	// Fills the whole (or part) of the image with the given color
	// Absence of the passed rect means filling the whole image
	void fill(
		Image *img,
		const color8 &color,
		const utils::rectu *rect = nullptr);

	// Copies the whole (or part) of the source image to the whole (or part) of the dest image
	/// @param "srcImg" - image whose the pixel data is copied
	/// @param "dstImg" - image where the copy happens to
	/// @param "srcRect" - part of the image which will be copied
	/// @param "dstRect" - part of the image where the copy will be done to
	/// @param "allowScale" - if true, the copy will occur with scaling before the "dstRect" bounds (upscaling or downscaling)
    bool copyTo(
        Image *srcImg,
		Image *dstImg,
		const utils::rectu *srcRect = nullptr,
		const utils::rectu *dstRect = nullptr,
		bool allowScale = false);

	// Scales (up or down) the image before the given rect.
	// The convolution algorithm is used with one of filter types.
    void resize(Image *img, const utils::rectu &rect, RESAMPLE_FILTER filter=RF_NEAREST);

	// Rotates the given image by the angle multiple by 90 degrees
	Image *rotate(Image *img, ROTATE_ANGLE angle);

	Image *flip(Image *img, FLIP_DIR dir);

    Image *crop(Image *img, const utils::rectu &rect);

    Image *combine(Image *img1, Image *img2);

	bool blendEnabled() const
	{
		return BlendEnabled;
	}

	void enableBlend(bool enable)
	{
		BlendEnabled = enable;
	}

    void setBlendMode(BlendMode newMode)
	{
		Mode = newMode;
	}
};

}
