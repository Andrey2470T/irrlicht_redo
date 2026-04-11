// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <vector>
#include <cstdint>

#include "SMaterialLayer.h"
#include "Logger.h"
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

class GLTexture : public virtual IReferenceCounted
{
public:
	struct SStatesCache
	{
		E_TEXTURE_CLAMP WrapU = ETC_REPEAT;
		E_TEXTURE_CLAMP WrapV = ETC_REPEAT;
		E_TEXTURE_CLAMP WrapW = ETC_REPEAT;
		u8 LODBias = 0;
		u8 AnisotropicFilter = 0;
		E_TEXTURE_MIN_FILTER MinFilter = ETMINF_NEAREST_MIPMAP_NEAREST;
		E_TEXTURE_MAG_FILTER MagFilter = ETMAGF_NEAREST;
		bool MipMapStatus = false;
		bool IsCached = false;
	};

	// Constructor from images
	GLTexture(const io::path &name, const std::vector<IImage *> &srcImages,
			E_TEXTURE_TYPE type, VideoDriver *driver);

	// Constructor for render target
	GLTexture(const io::path &name, const core::dimension2du &size,
			E_TEXTURE_TYPE type, ECOLOR_FORMAT format, VideoDriver *driver,
			u8 msaa = 0);

	~GLTexture();

	// ITexture interface methods
	void *lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE,
			u32 mipmapLevel = 0,
			u32 layer = 0,
			E_TEXTURE_LOCK_FLAGS lockFlags = ETLF_FLIP_Y_UP_RTT);

	void unlock();

	void regenerateMipMapLevels(u32 layer = 0);

	u32 getID() const;

	// Getters
	const core::dimension2du &getOriginalSize() const { return OriginalSize; }
	const core::dimension2du &getSize() const { return Size; }
	E_DRIVER_TYPE getDriverType() const { return DriverType; }
	ECOLOR_FORMAT getColorFormat() const { return ColorFormat; }
	ECOLOR_FORMAT getOriginalColorFormat() const { return OriginalColorFormat; }
	u32 getPitch() const { return Pitch; }
	bool hasMipMaps() const { return HasMipMaps; }
	bool isRenderTarget() const { return IsRenderTarget; }
	const io::SNamedPath &getName() const { return NamedPath; }
	E_TEXTURE_TYPE getType() const { return Type; }

	bool hasAlpha() const;

	SStatesCache &getStatesCache() const { return StatesCache; }

protected:
	// Helper function, helps to get the desired texture creation format from the flags.
	inline E_TEXTURE_CREATION_FLAG getTextureFormatFromFlags(u32 flags)
	{
		if (flags & ETCF_OPTIMIZED_FOR_SPEED)
			return ETCF_OPTIMIZED_FOR_SPEED;
		if (flags & ETCF_ALWAYS_16_BIT)
			return ETCF_ALWAYS_16_BIT;
		if (flags & ETCF_ALWAYS_32_BIT)
			return ETCF_ALWAYS_32_BIT;
		if (flags & ETCF_OPTIMIZED_FOR_QUALITY)
			return ETCF_OPTIMIZED_FOR_QUALITY;
		return ETCF_OPTIMIZED_FOR_SPEED;
	}

	static core::dimension2u getMipMapsSize(const core::dimension2du &baseSize,
											u32 mipLevel);

	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format);
	void getImageValues(const IImage *image);
	static void flipImageY(IImage *image);

	void initTexture(u32 layers);
	void uploadTexture(u32 layer, u32 level, void *data);
	u32 getTextureTarget(u32 layer) const;
	u32 textureTypeIrrToGL(E_TEXTURE_TYPE type) const;

	// Fields from ITexture
	io::SNamedPath NamedPath;
	core::dimension2du OriginalSize;
	core::dimension2du Size;
	ECOLOR_FORMAT OriginalColorFormat = ECF_UNKNOWN;
	ECOLOR_FORMAT ColorFormat = ECF_UNKNOWN;
	u32 Pitch = 0;
	bool HasMipMaps = false;
	bool IsRenderTarget = false;
	E_TEXTURE_TYPE Type;

	// Fields from COpenGLCoreTexture (stored as opaque handles)
	VideoDriver *Driver;
	u8 MSAA = 0;

	bool LockReadOnly = false;
	IImage *LockImage = nullptr;
	u32 LockLayer = 0;

	bool KeepImage = false;
	std::vector<IImage*> Images;

	u8 MipLevelStored = 0;

	mutable SStatesCache StatesCache;
};

} // end namespace video
