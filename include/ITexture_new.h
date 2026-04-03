// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "IReferenceCounted.h"
#include "SColor.h"
#include "dimension2d.h"
#include "EDriverTypes.h"
#include "SMaterialLayer.h"
#include "path.h"


namespace video
{

//! Enumeration flags used to tell the video driver with setTextureCreationFlag in which format textures should be created.
enum E_TEXTURE_CREATION_FLAG
{
	/** Forces the driver to create 16 bit textures always, independent of
	which format the file on disk has. When choosing this you may lose
	some color detail, but gain much speed and memory. 16 bit textures can
	be transferred twice as fast as 32 bit textures and only use half of
	the space in memory.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_32_BIT, ETCF_OPTIMIZED_FOR_QUALITY, or
	ETCF_OPTIMIZED_FOR_SPEED at the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_ALWAYS_16_BIT = 0x00000001,

	/** Forces the driver to create 32 bit textures always, independent of
	which format the file on disk has. Please note that some drivers (like
	the software device) will ignore this, because they are only able to
	create and use 16 bit textures.
	Default is true.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_16_BIT, ETCF_OPTIMIZED_FOR_QUALITY, or
	ETCF_OPTIMIZED_FOR_SPEED at the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_ALWAYS_32_BIT = 0x00000002,

	/** Lets the driver decide in which format the textures are created and
	tries to make the textures look as good as possible. Usually it simply
	chooses the format in which the texture was stored on disk.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_SPEED at
	the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_OPTIMIZED_FOR_QUALITY = 0x00000004,

	/** Lets the driver decide in which format the textures are created and
	tries to create them maximizing render speed.
	When using this flag, it does not make sense to use the flags
	ETCF_ALWAYS_16_BIT, ETCF_ALWAYS_32_BIT, or ETCF_OPTIMIZED_FOR_QUALITY,
	at the same time.
	Not all texture formats are affected (usually those up to ECF_A8R8G8B8). */
	ETCF_OPTIMIZED_FOR_SPEED = 0x00000008,

	/** Creates textures with mipmap levels.
	If disabled textures can not have mipmaps.
	Default is true. */
	ETCF_CREATE_MIP_MAPS = 0x00000010,

	/** Discard any alpha layer and use non-alpha color format.
	Warning: This may lead to getting 24-bit texture formats which
			 are often badly supported by drivers. So it's generally
			 not recommended to enable this flag.	*/
	ETCF_NO_ALPHA_CHANNEL = 0x00000020,

	//! Allow the driver to keep a copy of the texture in memory
	/** Enabling this makes calls to ITexture::lock a lot faster, but costs main memory.
	This is disabled by default.
	*/
	ETCF_ALLOW_MEMORY_COPY = 0x00000080,

	/** This flag is never used, it only forces the compiler to compile
	these enumeration values to 32 bit. */
	ETCF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

//! Enumeration describing the type of ITexture.
enum E_TEXTURE_TYPE
{
	//! 2D texture.
	ETT_2D = 0,

	//! 2D texture with multisampling.
	ETT_2D_MS,

	//! Cubemap texture.
	ETT_CUBEMAP,

	ETT_COUNT
};

//! Enumeration of each cubemap face
enum E_CUBEMAP_FACE
{
	ECMF_POS_X = 0,
	ECMF_NEG_X,
	ECMF_POS_Y,
	ECMF_NEG_Y,
	ECMF_POS_Z,
	ECMF_NEG_Z,
	ECMF_COUNT
};

struct TextureSettings
{
	E_TEXTURE_CLAMP wrapU = ETC_REPEAT;
	E_TEXTURE_CLAMP wrapV = ETC_REPEAT;
	E_TEXTURE_CLAMP wrapW = ETC_REPEAT;

	E_TEXTURE_MIN_FILTER minF = ETMINF_NEAREST_MIPMAP_NEAREST;
	E_TEXTURE_MAG_FILTER magF = ETMAGF_NEAREST;

	f32 lodBias = 0.0f;
	u8 anisotropyFilter = 0;

	bool isRenderTarget = false;
	bool hasMipMaps = false;
	u8 maxMipLevel = 0;

	f32 maxLodBias = 0.0f;
	u8 maxAnisotropyFilter = 0;
};

class IImage;
class VideoDriver;

class Texture
{
protected:
	video::VideoDriver *driver;

	E_TEXTURE_TYPE type;
	u32 texID;
	std::string name;

	core::dimension2du originalSize;
	core::dimension2du size;

	ECOLOR_FORMAT originalColorFormat = ECF_UNKNOWN;
	ECOLOR_FORMAT colorFormat = ECF_UNKNOWN;
	u32 pitch = 0;
	u8 msaa = 0;

	TextureSettings texSettings;

	std::vector<IImage *> imgCache;
	bool cacheImages = false;
public:
	Texture(video::VideoDriver *_driver, E_TEXTURE_TYPE _type, const std::string &_name,
		  const core::dimension2du &_size, ECOLOR_FORMAT _format, u8 _msaa=0);
    Texture(video::VideoDriver *_driver, E_TEXTURE_TYPE _type, const std::string &_name,
		  const std::vector<IImage *> &_images, const TextureSettings &_settings=TextureSettings());

	~Texture();

	//! Get name of texture (in most cases this is the filename)
	const std::string &getName() const { return name; }

	//! Returns the type of texture
	E_TEXTURE_TYPE getType() const { return type; };

	u32 getID() const { return texID; }

	//! Get original size of the texture.
	/** The texture is usually scaled, if it was created with an unoptimal
	size. For example if the size was not a power of two. This method
	returns the size of the texture it had before it was scaled. Can be
	useful when drawing 2d images on the screen, which should have the
	exact size of the original texture. Use ITexture::getSize() if you want
	to know the real size it has now stored in the system.
	\return The original size of the texture. */
	const core::dimension2du &getOriginalSize() const { return originalSize; };

	//! Get dimension (=size) of the texture.
	/** \return The size of the texture. */
	const core::dimension2du &getSize() const { return size; };

	//! Get the color format of texture.
	/** \return The color format of texture. */
	ECOLOR_FORMAT getColorFormat() const { return colorFormat; };

	//! Get the original color format
	/** When create textures from image data we will often use different color formats.
	For example depending on driver TextureCreationFlag's.
	This can give you the original format which the image used to create the texture had	*/
	ECOLOR_FORMAT getOriginalColorFormat() const { return originalColorFormat; };

	//! Get pitch of the main texture (in bytes).
	/** The pitch is the amount of bytes used for a row of pixels in a
	texture.
	\return Pitch of texture in bytes. */
	u32 getPitch() const { return pitch; };

	//! Check whether the texture has MipMaps
	/** \return True if texture has MipMaps, else false. */
	bool hasMipMaps() const { return texSettings.hasMipMaps; }

	//! Check whether the texture is a render target
	/** Render targets can be set as such in the video driver, in order to
	render a scene into the texture. Once unbound as render target, they can
	be used just as usual textures again.
	\return True if this is a render target, otherwise false. */
	bool isRenderTarget() const { return texSettings.isRenderTarget; }

	//! Returns if the texture has an alpha channel
	bool hasAlpha() const { return pixelFormatsInfo[colorFormat].hasAlpha; }

	const TextureSettings &getParameters() const { return texSettings; }

	void bind() const;
	void unbind() const;

	void uploadData(IImage *img, u8 mipLevel=0, u8 layer=0);
	void uploadSubData(u32 x, u32 y, IImage *img, u8 mipLevel=0, u8 layer=0);

	IImage * downloadData(u8 mipLevel=0, u8 layer=0);
	void regenerateMipMaps();

	// NOTE: Will be defined later
	//void updateParameters(
	//	const TextureSettings &newTexSettings,
	//	bool updateLodBias, bool updateAnisotropy);

	//void resize(const core::dimension2du &newSize);
	//ITexture *copy(const std::string &name="");

	bool operator==(const Texture &other) const;

protected:
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);
	void getParametersFromImage(const IImage *image);

	core::dimension2u getMipMapSize(u8 mipLevel);

	void initTexture(u8 layers);
};

} // end namespace video
