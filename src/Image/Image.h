#pragma once

#include "Render/Common.h"
#include "ImageFormats.h"
#include "Utils/Rect.h"
#include "Color.h"

namespace img {

/* Image representation class referring or saving directly
 * the pixel data. Can be RGB or RGBA unsigned integral 8-bit format.
*/
class Image
{
	ImageFormat format;

	u32 width;
	u32 height;

	u8* data;

	bool ownPixelData;
public:
	Image(ImageFormat _format, u32 _width, u32 _height);

	Image(ImageFormat _format, u32 _width, u32 _height, u8 *data, bool copyData = true);

	~Image();

	ImageFormat getFormat() const
	{
		return format;
	}

	u32 getWidth() const
	{
		return width;
	}

	u32 getHeight() const
	{
		return height;
	}

	u8* getData() const
	{
		return data.get();
	}

	//! Returns a pixel
	ColorRGBA<u8> getPixel(u32 x, u32 y) const;

	//! Sets a pixel
	void setPixel(u32 x, u32 y, const ColorRGBA<u8> &color, bool blend = false);

	//! Copies this surface into another, if it has the exact same size and format.
	/**	NOTE: mipmaps are ignored
	\return True if it was copied, false otherwise.
	*/
	bool copyToNoScaling(void *target, u32 width, u32 height, ImageFormat format = IF_RGBA8, u32 pitch = 0) const;

	//! Copies the image into the target, scaling the image to fit
	/**	NOTE: mipmaps are ignored */
	void copyToScaling(void *target, u32 width, u32 height, ImageFormat format = IF_RGBA8, u32 pitch = 0);

	//! Copies the image into the target, scaling the image to fit
	/**	NOTE: mipmaps are ignored */
	void copyToScaling(Image *target);

	//! copies this surface into another
	/**	NOTE: mipmaps are ignored */
	void copyTo(Image *target, const Vector2D<s32> &pos = Vector2D<s32>(0, 0));

	//! copies this surface into another
	/**	NOTE: mipmaps are ignored */
	void copyTo(Image *target, const Vector2D<s32> &pos, const Rect<s32> &sourceRect, const Rect<s32> *clipRect = 0);

	//! copies this surface into another, using the alpha mask and cliprect and a color to add with
	/**	NOTE: mipmaps are ignored
	\param combineAlpha - When true then combine alpha channels. When false replace target image alpha with source image alpha.
	*/
	void copyToWithAlpha(Image *target, const Vector2D<s32> &pos,
			const Rect<s32> &sourceRect, const ColorRGBA<u8> &color,
			const Rect<s32> *clipRect = 0,
			bool combineAlpha = false);

	//! fills the whole image or a subimage clipped by 'rect' with given color
	void fill(const ColorRGBA<u8> &color, const Rect<u32> *rect = nullptr);
};

}
