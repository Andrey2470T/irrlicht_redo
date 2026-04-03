#include "Texture.h"
#include "Common.h"
#include "VideoDriver.h"
#include "Logger.h"

namespace render
{

std::array<GLenum, ETT_COUNT> toGLTexType = {
	GL_TEXTURE_2D,
	GL_TEXTURE_2D_MULTISAMPLE,
	GL_TEXTURE_CUBE_MAP
};

GLenum getTextureTarget(E_TEXTURE_TYPE type, u32 layer) const
{
	GLenum tmp = toGLTexType[type];
	if (tmp == GL_TEXTURE_CUBE_MAP) {
		assert(layer < 6);
		tmp = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
	}
	return tmp;
}

Texture::Texture(
	video::VideoDriver *_driver, E_TEXTURE_TYPE _type, const std::string &_name,
	const core::dimension2du &_size, ECOLOR_FORMAT _format, u8 _msaa)
	: driver(_driver), type(_type), name(_name), originalSize(_size), size(_size),
	  originalColorFormat(_format), colorFormat(_format), texSettings(_texSettings), msaa(_msaa)
{
	texSettings.isRenderTarget = true;

	if (originalColorFormat == ECF_UNKNOWN)
		colorFormat = getBestColorFormat(driver->getColorFormat());

	pitch = size.Width * pixelFormatsInfo[colorFormat].size / 8;

	// On GLES 3.0 we must use sized internal formats for textures when calling
	// glTexStorage. But ECF_A8R8G8B8 might be mapped to GL_BGRA (an unsized format).
	// Since we don't upload to RTT we can safely pick a different combo that works.
	//if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3) {
	//	InternalFormat = GL_RGBA8;
	//	PixelFormat = GL_RGBA;
	//}

	initTexture({});
}

Texture2D::Texture2D(
	video::VideoDriver *_driver, E_TEXTURE_TYPE _type, const std::string &_name,
	const std::vector<IImage *> &_images, const TextureSettings &_settings)
	: driver(_driver), type(_type), name(_name), texSettings(_texSettings)
{
	assert(!_images.empty() && _images.size() <= 6);
	texSettings.hasMipMaps = driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS) || texSettings.hasMipMaps;
	cacheImages = driver->getTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY);

	getParametersFromImage(_images[0]);

	auto &tmpImages = _images;

	if (cacheImages || originalSize != size || originalColorFormat != colorFormat) {
		imgCache.resize(tmpImages.size());

		for (u8 i = 0; i < tmpImages.size(); i++) {
			imgCache[i] = driver->createImage(colorFormat, size);

			if (tmpImages[i]->getDimension() == size)
				tmpImages[i]->copyTo(imgCache[i]);
			else
				tmpImages[i]->copyToScaling(imgCache[i]);

			tmpImages = imgCache;
		}
	}

	initTexture(tmpImages.size());

	for (auto &tmpImg : tmpImages)
		uploadData(tmpImg);

	if (texSettings.hasMipMaps)
		regenerateMipMapLevels();

	if (!cacheImages) {
		for (auto &img : imgCache)
			img->drop();

		imgCache.clear();
	}
}

Texture::~Texture()
{
	glDeleteTextures(1, &texID);
	driver->testGLError();

	for (auto &img : imgCache)
		img->drop();
}

void Texture::bind()
{
	glBindTexture(toGLTexType[type], texID);
	driver->testGLError();
}

void Texture::unbind()
{
	glBindTexture(toGLTexType[type], 0);
	driver->testGLError();
}

void Texture::uploadData(img::Image *img, u8 mipLevel, u8 layer)
{
	uploadSubData(width, height, img, mipLevel);
}

void Texture::uploadSubData(u32 x, u32 y, img::Image *img, u8 mipLevel, u8 layer)
{
	if (!img)
		return;

	auto &formatInfo = textureFormats[colorFormat];
	auto tmpImg = img;

	// Convert the data from the base (0-th) mip level to the 'mipLevel'
	if (formatInfo.Converter) {
		auto tmpImgSize = getMipMapSize(mipLevel);

		tmpImg = new CImage(colorFormat, tmpImgSize);
		formatInfo.Converter(img->getData(), tmpImgSize.getArea(), tmpImg->getData());
	}

	auto ctxt = Driver->getContext();

	auto prevTexture = ctxt->getTextureUnit(0);
	ctxt->setTextureUnit(0, this);

	// NOTE: 'x' and 'y' are within the given mipLevel, 'img' should be the base mip`s image
	glTexSubImage2D(getTextureTarget(type, layer), mipLevel, x, y, width, height,
		formatInfo.PixelFormat, formatInfo.PixelType, tmpImg->getData());

	driver->testGLError();

	ctxt->setTextureUnit(0, prevTexture);

	if (formatInfo.Converter)
		delete tmpImg;
}

img::Image * Texture::downloadData(u8 mipLevel, u8 layer)
{
	if (cacheImages && mipLevel == 0) {
		assert(layer < imgCache.size());
		return imgCache[layer];
	}

	auto tmpImgSize = getMipMapSize(mipLevel);
	auto tmpImg = new CImage(colorFormat, tmpImgSize);

#ifdef _IRR_COMPILE_WITH_OPENGL3_
	auto ctxt = Driver->getContext();

	auto prevTexture = ctxt->getTextureUnit(0);
	ctxt->setTextureUnit(0, this);

	auto &formatInfo = textureFormats[colorFormat];

	glGetTexImage(getTextureTarget(type, layer), mipLevel, formatInfo.PixelFormat, formatInfo.PixelType, tmpImg->getData());
	driver->testGLError();

	if (texSettings.isRenderTarget)
		tmpImg->flipY();

	ctxt->setTextureUnit(0, prevTexture);
#else
	auto tmpFBO = new RenderTarget(driver);
	auto prevFBO = ctxt->getRenderTarget();
	ctxt->setRenderTarget(tmpFBO);

	// Warning: on GLES 2.0 this call will only work with mipmapLevel == 0
	tmpFBO->setColorTextures({this}, {}, mipLevel);

	auto tmpImg2 = driver->createImage(ECF_A8R8G8B8, tmpImgSize);
	glReadPixels(0, 0, tmpImgSize.Width, tmpImgSize.Height,
		GL_RGBA, GL_UNSIGNED_BYTE, tmpImg2->getData());

	tmpFBO->setColorTextures({}, {});

	ctxt->setRenderTarget(prevFBO);

	tmpFBO->drop();

	if (texSettings.isRenderTarget)
		tmpImg2->flipY();

	void *src = tmpImg2->getData();
	void *dest = tmpImg->getData();

	switch (colorFormat) {
	case ECF_A1R5G5B5:
		CColorConverter::convert_A8R8G8B8toA1B5G5R5(src, tmpImgSize.getArea(), dest);
		break;
	case ECF_R5G6B5:
		CColorConverter::convert_A8R8G8B8toR5G6B5(src, tmpImgSize.getArea(), dest);
		break;
	case ECF_R8G8B8:
		CColorConverter::convert_A8R8G8B8toB8G8R8(src, tmpImgSize.getArea(), dest);
		break;
	case ECF_A8R8G8B8:
		CColorConverter::convert_A8R8G8B8toA8B8G8R8(src, tmpImgSize.getArea(), dest);
		break;
	default:
		tmpImg->drop();
		tmpImg = nullptr;
		break;
	}

	tmpImg2->drop();
#endif

	return tmpImg;
}

void Texture2D::regenerateMipMaps()
{
	if (!texSettings.hasMipMaps) {
		g_irrlogger->log("Texture2D::regenerateMipMaps() mip maps are disabled", ELL_ERROR);
		return;
	}

	auto ctxt = Driver->getContext();

	auto prevTexture = ctxt->getTextureUnit(0);
	ctxt->setTextureUnit(0, this);

	glGenerateMipmap(toGLTexType[type]);
	driver->testGLError();

	ctxt->setTextureUnit(0, prevTexture);
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

	originalSize = image->getDimension();
	size = originalSize;

	if (size.Width == 0 || size.Height == 0) {
		g_irrlogger->log("Invalid size of image for texture.", ELL_ERROR);
		return;
	}

	const f32 ratio = (f32)size.Width / (f32)size.Height;

	auto features = driver->getFeatures();
	if ((size.Width > features.MaxTextureSize) && (ratio >= 1.0f)) {
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

core::dimension2u ITexture::getMipMapSize(u8 mipLevel)
{
	u32 width = size.Width >> mipLevel;
	u32 height = size.Height >> mipLevel;
	if (width < 1)
		width = 1;
	if (height < 1)
		height = 1;

	return {width, height};
}

void Texture::initTexture(u8 layers)
{
	glGenTextures(1, &texID);
	driver->testGLError();

	auto ctxt = Driver->getContext();

	auto prevTexture = ctxt->getTextureUnit(0);
	ctxt->setTextureUnit(0, this);

	auto &formatInfo = GLSpecificInfo::TextureFormats[format];

	if (texSettings.isRenderTarget) {
		// An INVALID_ENUM error is generated by TexParameter* if target is either
		// TEXTURE_2D_MULTISAMPLE or TEXTURE_2D_MULTISAMPLE_ARRAY, and pname is any
		// sampler state from table 23.18.
		// ~ https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf
		if (type != ETT_2D_MS) {
			glTexParameteri(toGLTexType[type], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(toGLTexType[type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(toGLTexType[type], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(toGLTexType[type], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(toGLTexType[type], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

			texSettings.minF = ETMINF_NEAREST;
			texSettings.wrapU = ETC_CLAMP_TO_EDGE;
			texSettings.wrapV = ETC_CLAMP_TO_EDGE;
			texSettings.wrapW = ETC_CLAMP_TO_EDGE;
		}

		driver->testGLError();
	}
	else {
		glTexParameteri(toGLTexType[type], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(toGLTexType[type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		texSettings.minF = ETMINF_NEAREST;

		if (texSettings.hasMipMaps) {
			if (driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
			else if (driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
				glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
			else
				glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);
		}
		Driver->testGLError();
	}

	if (texSettings.hasMipMaps && texSettings.maxMipLevel == 0)
		texSettings.maxMipLevel = core::u32_log2(core::max_(size.Width, size.Height)) + 1;

	// reference: <https://www.khronos.org/opengl/wiki/Texture_Storage>
	bool use_tex_storage = driver->getFeatures().TexStorage;

	// On GLES 3.0 if we don't have a sized format suitable for glTexStorage,
	// just avoid using it. Only affects the extension that provides BGRA.
	//if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3)
	//	use_tex_storage = false;

	if (type != ETT_2D_MS) {
		for (u8 i = 0; i < layers; i++) {
			if (use_tex_storage) {
				glTexStorage2D(getTextureTarget(type, i), texSettings.maxMipLevel, formatInfo.InternalFormat,
					size.Width, size.Height);
			} else {
				glTexImage2D(getTextureTarget(type, i), 0, formatInfo.InternalFormat,
					size.Width, size.Height, 0, formatInfo.PixelFormat, formatInfo.PixelType, 0);
			}
			driver->testGLError();
		}
	}
	else {
		GLint max_samples = 0;
		glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
		msaa = core::min_(msaa, (u8)max_samples);

		assert(msaa > 0);

		// glTexImage2DMultisample is supported by OpenGL 3.2+
		// glTexStorage2DMultisample is supported by OpenGL 4.3+ and OpenGL ES 3.1+
		// so pick the most compatible one
		const bool use_gl_impl = driver->getVersion().Spec != OpenGLSpec::ES;

		if (use_gl_impl)
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, formatInfo.InternalFormat,
				size.Width, size.Height, GL_TRUE);
		else
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, formatInfo.InternalFormat,
				size.Width, size.Height, GL_TRUE);
		driver->testGLError();
	}

	if (!name.empty())
		driver->GLInfo->ObjectLabel(GL_TEXTURE, texID, name.c_str());

	ctxt->setTextureUnit(0, prevTexture);
}

}
