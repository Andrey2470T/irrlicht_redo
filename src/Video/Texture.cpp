#include "ITexture.h"
#include "Common.h"
#include "VideoDriver.h"
#include "Logger.h"

namespace render
{

ITexture::ITexture(video::VideoDriver *_driver, const std::string &_name,
	const core::dimension2du &_size, ECOLOR_FORMAT _format,
	const TextureSettings &_texSettings)
	: driver(_driver), name(_name), originalSize(_size), size(_size),
	  originalColorFormat(_format), colorFormat(_format), texSettings(_texSettings)
{}

ITexture::~ITexture()
{
	glDeleteTextures(1, &texID);
    driver->testGLError();
}

//! Returns if the texture has an alpha channel
bool ITexture::hasAlpha() const
{
	bool status = false;

	switch (colorFormat) {
	case ECF_A8R8G8B8:
	case ECF_A1R5G5B5:
	case ECF_A16B16G16R16F:
	case ECF_A32B32G32R32F:
		status = true;
		break;
	default:
		break;
	}

	return status;
}

bool ITexture::operator==(const ITexture &other) const
{
    return texID == other.texID;
}

ECOLOR_FORMAT ITexture::getBestColorFormat(ECOLOR_FORMAT format)
{
	// We only try for to adapt "simple" formats
	ECOLOR_FORMAT destFormat = (format <= ECF_A8R8G8B8) ? ECF_A8R8G8B8 : format;

	switch (format) {
	case ECF_A1R5G5B5:
		if (!driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
			destFormat = ECF_A1R5G5B5;
		break;
	case ECF_R5G6B5:
		if (!driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
			destFormat = ECF_R5G6B5;
		break;
	case ECF_A8R8G8B8:
		if (driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
				driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
			destFormat = ECF_A1R5G5B5;
		break;
	case ECF_R8G8B8:
		// Note: Using ECF_A8R8G8B8 even when ETCF_ALWAYS_32_BIT is not set as 24 bit textures fail with too many cards
		if (driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) || driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
			destFormat = ECF_A1R5G5B5;
	default:
		break;
	}

	if (driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL)) {
		switch (destFormat) {
		case ECF_A1R5G5B5:
			destFormat = ECF_R5G6B5;
			break;
		case ECF_A8R8G8B8:
			destFormat = ECF_R8G8B8;
			break;
		default:
			break;
		}
	}

	return destFormat;
}

void ITexture::getParametersFromImage(const IImage *image)
{
	originalColorFormat = image->getColorFormat();
	colorFormat = getBestColorFormat(originalColorFormat);

	if (IImage::isCompressedFormat(image->getColorFormat())) {
		copyCache = false;
	}

	originalSize = image->getDimension();
	size = originalSize;

	if (size.Width == 0 || size.Height == 0) {
		g_irrlogger->log("Invalid size of image for texture.", ELL_ERROR);
		return;
	}

	const f32 ratio = (f32)size.Width / (f32)size.Height;

	auto features = driver->getFeatures();
	if ((size.Width > features.MaxTextureSize) && (ratio >= 1.f)) {
		size.Width = features.MaxTextureSize;
		size.Height = (u32)(features.MaxTextureSize / ratio);
	} else if (size.Height > features.MaxTextureSize) {
		size.Height = features.MaxTextureSize;
		size.Width = (u32)(features.MaxTextureSize * ratio);
	}

	//bool needSquare = (Type == ETT_CUBEMAP);

	size = size.getOptimalSize(false, false, true, features.MaxTextureSize);

	pitch = size.Width * IImage::getBitsPerPixelFromFormat(colorFormat) / 8;
}

void ITexture::flipImageY(IImage *image)
{
	const u32 pitch = image->getPitch();
	u8 *srcA = static_cast<u8 *>(image->getData());
	u8 *srcB = srcA + (image->getDimension().Height - 1) * pitch;

	std::vector<u8> tmpBuffer(pitch);
	for (u32 i = 0; i < image->getDimension().Height; i += 2) {
		memcpy(tmpBuffer.data(), srcA, pitch);
		memcpy(srcA, srcB, pitch);
		memcpy(srcB, tmpBuffer.data(), pitch);
		srcA += pitch;
		srcB -= pitch;
	}
}

}
