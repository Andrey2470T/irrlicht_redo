// This file is part of the "Irrlicht Engine".
// written by Reinhard Ostermeier, reinhard@nospam.r-ostermeier.de

#pragma once

#include "GUI/IGUIImageList.h"
#include "Video/VideoDriver.h"


namespace gui
{

class CGUIImageList : public IGUIImageList
{
public:
	//! constructor
	CGUIImageList(video::VideoDriver *Driver);

	//! destructor
	virtual ~CGUIImageList();

	//! Creates the image list from texture.
	//! \param texture: The texture to use
	//! \param imageSize: Size of a single image
	//! \param useAlphaChannel: true if the alpha channel from the texture should be used
	//! \return
	//! true if the image list was created
	bool createImageList(video::GLTexture *texture,
			core::dimension2d<s32> imageSize,
			bool useAlphaChannel);

	//! Draws an image and clips it to the specified rectangle if wanted
	//! \param index: Index of the image
	//! \param destPos: Position of the image to draw
	//! \param clip: Optional pointer to a rectangle against which the text will be clipped.
	//! If the pointer is null, no clipping will be done.
	virtual void draw(s32 index, const core::position2d<s32> &destPos,
			const core::rect<s32> *clip = 0) override;

	//! Returns the count of Images in the list.
	//! \return Returns the count of Images in the list.
	s32 getImageCount() const override
	{
		return ImageCount;
	}

	//! Returns the size of the images in the list.
	//! \return Returns the size of the images in the list.
	core::dimension2d<s32> getImageSize() const override
	{
		return ImageSize;
	}

private:
	video::VideoDriver *Driver;
	video::GLTexture *Texture;
	s32 ImageCount;
	core::dimension2d<s32> ImageSize;
	s32 ImagesPerRow;
	bool UseAlphaChannel;
};

} // end namespace gui
