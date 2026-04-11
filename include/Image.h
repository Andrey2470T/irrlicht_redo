// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IReferenceCounted.h"
#include "position2d.h"
#include "rect.h"
#include "SColor.h"
#include <cstring>
#include "path.h"

namespace io
{
class IReadFile;
class IWriteFile;
class IFileSystem;
}

namespace video
{

enum E_FLIP_AXIS
{
    EFA_X,
    EFA_Y
};

//! check sanity of image dimensions to prevent issues later, for use by CImageLoaders
inline bool checkImageDimensions(u32 width, u32 height)
{
	// 4 * 23000 * 23000 is just under S32_MAX
	return width <= 23000 && height <= 23000;
}

//! Interface for software image data.
/** Image loaders create these images from files. IVideoDrivers convert
these images into their (hardware) textures.
NOTE: Floating point formats are not well supported yet. Basically only getData() works for them.
*/
class Image : public virtual IReferenceCounted
{
    ECOLOR_FORMAT Format;
    core::dimension2d<u32> Size;

    u8 *Data = nullptr;

    u32 BytesPerPixel = 0;
    u32 Pitch = 0;

    bool DeleteMemory;
public:
	//! constructor from raw image data
	/** \param useForeignMemory: If true, the image will use the data pointer
	directly and own it from now on, which means it will also try to delete [] the
	data when the image will be destructed. If false, the memory will by copied. */
    Image(ECOLOR_FORMAT format, const core::dimension2d<u32> &size, void *data,
			bool ownForeignMemory = true, bool deleteMemory = true);

	//! constructor for empty image
    Image(ECOLOR_FORMAT format, const core::dimension2d<u32> &size);

    //! destructor
    ~Image();

    //! Returns the color format
    ECOLOR_FORMAT getColorFormat() const
    {
        return Format;
    }

    //! Returns width and height of image data.
    const core::dimension2d<u32> &getDimension() const
    {
        return Size;
    }

    //! Returns bits per pixel.
    u32 getBitsPerPixel() const
    {
        return pixelFormatsInfo[Format].size;
    }

    //! Returns bytes per pixel
    u32 getBytesPerPixel() const
    {
        return BytesPerPixel;
    }

    //! Returns image data size in bytes
    u32 getImageDataSizeInBytes() const
    {
        return getDataSizeFromFormat(Format, Size.Width, Size.Height);
    }

    //! Returns image data size in pixels
    u32 getImageDataSizeInPixels() const
    {
        return Size.Width * Size.Height;
    }

    //! Returns pitch of image
    u32 getPitch() const
    {
        return Pitch;
    }

    //! Use this to get a pointer to the image data.
    /**
    \return Pointer to the image data. What type of data is pointed to
    depends on the color format of the image. For example if the color
    format is ECF_A8R8G8B8, it is of u32. */
    void *getData() const
    {
        return Data;
    }

	//! returns a pixel
    SColor getPixel(u32 x, u32 y) const;

	//! sets a pixel
    void setPixel(u32 x, u32 y, const SColor &color, bool blend = false);

	//! copies this surface into another, if it has the exact same size and format.
    bool copyToNoScaling(void *target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch = 0) const;

	//! copies this surface into another, scaling it to fit.
    void copyToScaling(void *target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch = 0);

	//! copies this surface into another, scaling it to fit.
    void copyToScaling(Image *target);

	//! copies this surface into another
    void copyTo(Image *target, const core::position2d<s32> &pos = core::position2d<s32>(0, 0));

	//! copies this surface into another
    void copyTo(Image *target, const core::position2d<s32> &pos, const core::rect<s32> &sourceRect, const core::rect<s32> *clipRect = 0);

	//! copies this surface into another, using the alpha mask, an cliprect and a color to add with
    virtual void copyToWithAlpha(Image *target, const core::position2d<s32> &pos,
			const core::rect<s32> &sourceRect, const SColor &color,
            const core::rect<s32> *clipRect = 0, bool combineAlpha = false);

	//! copies this surface into another, scaling it to fit, applying a box filter
    void copyToScalingBoxFilter(Image *target, s32 bias = 0, bool blend = false);

	//! fills the surface with given color
    void fill(const SColor &color);

    void flip(E_FLIP_AXIS axis);

private:
	inline SColor getPixelBox(s32 x, s32 y, s32 fx, s32 fy, s32 bias) const;
};

} // end namespace video
