// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <vector>
#include <cstdint>

#include "IReferenceCounted.h"
#include "SMaterialLayer.h"
#include "dimension2d.h"
#include "EDriverTypes.h"
#include "path.h"
#include "SColor.h"

// Forward declarations
namespace video
{
	class VideoDriver;
	class IImage;
	class CImage;
}

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
	/** Enabling this makes calls to Texture::lock a lot faster, but costs main memory.
	This is disabled by default.
	*/
	ETCF_ALLOW_MEMORY_COPY = 0x00000080,

	/** This flag is never used, it only forces the compiler to compile
	these enumeration values to 32 bit. */
	ETCF_FORCE_32_BIT_DO_NOT_USE = 0x7fffffff
};

//! Enumeration describing the type of Texture.
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

//! Enum for the mode for texture locking. Read-Only, write-only or read/write.
enum E_TEXTURE_LOCK_MODE
{
	//! The default mode. Texture can be read and written to.
	ETLM_READ_WRITE = 0,

	//! Read only. The texture is downloaded, but not uploaded again.
	/** Often used to read back shader generated textures. */
	ETLM_READ_ONLY,

	//! Write only. The texture is not downloaded and might be uninitialized.
	/** The updated texture is uploaded to the GPU.
	Used for initializing the shader from the CPU. */
	ETLM_WRITE_ONLY
};

struct TextureSettings
{
    E_TEXTURE_CLAMP WrapU = ETC_REPEAT;
    E_TEXTURE_CLAMP WrapV = ETC_REPEAT;
    E_TEXTURE_CLAMP WrapW = ETC_REPEAT;

    E_TEXTURE_MIN_FILTER MinF = ETMINF_NEAREST_MIPMAP_NEAREST;
    E_TEXTURE_MAG_FILTER MagF = ETMAGF_NEAREST;

    f32 LodBias = 0.0f;
    u8 AnisotropyFilter = 0;

    bool IsRenderTarget = false;
    bool HasMipMaps = false;
    u8 MaxMipLevel = 0;
};

class GLTexture : public virtual IReferenceCounted
{
public:
	// Constructor from images
    GLTexture(const io::path &name, const std::vector<IImage *> &srcImages,
              E_TEXTURE_TYPE type, VideoDriver *driver, const TextureSettings &settings=TextureSettings());

	// Constructor for render target
	GLTexture(const io::path &name, const core::dimension2du &size,
			E_TEXTURE_TYPE type, ECOLOR_FORMAT format, VideoDriver *driver,
			u8 msaa = 0);

	~GLTexture();

    void bind() const;
    void unbind() const;

	// Texture interface methods
	void *lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE,
            u32 mipLevel = 0,
			u32 layer = 0);

	void unlock();

    void regenerateMipMaps();

    u32 getID() const { return TexID; }

	// Getters
	const core::dimension2du &getOriginalSize() const { return OriginalSize; }
	const core::dimension2du &getSize() const { return Size; }

	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	ECOLOR_FORMAT getOriginalColorFormat() const { return OriginalColorFormat; }

	u32 getPitch() const { return Pitch; }
    bool hasMipMaps() const { return TexSettings.HasMipMaps; }
    bool isRenderTarget() const { return TexSettings.IsRenderTarget; }

	const io::SNamedPath &getName() const { return NamedPath; }
	E_TEXTURE_TYPE getType() const { return Type; }

    bool hasAlpha() const { return pixelFormatsInfo[ColorFormat].hasAlpha; }

    const TextureSettings &getParameters() const { return TexSettings; }

protected:
    core::dimension2du getMipMapsSize(u32 mipLevel);
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);
	void getImageValues(const IImage *image);

    void genTexture();
    void initTexture();
	void uploadTexture(u32 layer, u32 level, void *data);

	// Fields from Texture
	io::SNamedPath NamedPath;
	core::dimension2du OriginalSize;
	core::dimension2du Size;
	ECOLOR_FORMAT OriginalColorFormat = ECF_UNKNOWN;
	ECOLOR_FORMAT ColorFormat = ECF_UNKNOWN;
	u32 Pitch = 0;
	E_TEXTURE_TYPE Type;

	VideoDriver *Driver;
	u32 TexID;
	u8 MSAA = 0;

	bool LockReadOnly = false;
	IImage *LockImage = nullptr;
	u32 LockLayer = 0;
    u8 LockMipLevel = 0;

	bool KeepImage = false;
	std::vector<IImage*> Images;

    TextureSettings TexSettings;
};

} // end namespace video
