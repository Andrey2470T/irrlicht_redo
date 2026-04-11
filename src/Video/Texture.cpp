// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Texture.h"
#include "VideoDriver.h"
#include "Image/CImage.h"
#include "Image/CColorConverter.h"
#include "GLSpecificInfo.h"
#include "Logger.h"


namespace video
{

//=============================================================================
// Construction / Destruction
//=============================================================================

GLTexture::GLTexture(const io::path &name, const std::vector<IImage *> &srcImages,
                     E_TEXTURE_TYPE type, VideoDriver *driver) :
	NamedPath(name),
	DriverType(driver->getDriverType()),
	Type(type),
	Driver(driver)
{
	assert(!srcImages.empty() || Type != ETT_2D_MS);

	HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	KeepImage = Driver->getTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY);

	getImageValues(srcImages[0]);

	auto &formatInfo = GLSpecificInfo::TextureFormats[ColorFormat];

	char lbuf[128];
	snprintf_irr(lbuf, sizeof(lbuf),
		"GLTexture: Type = %d Size = %dx%d (%dx%d) ColorFormat = %d (%d)%s -> %#06x %#06x %#06x%s",
		(int)Type, Size.Width, Size.Height, OriginalSize.Width, OriginalSize.Height,
		(int)ColorFormat, (int)OriginalColorFormat,
		HasMipMaps ? " +Mip" : "",
		formatInfo.InternalFormat, formatInfo.PixelFormat, formatInfo.PixelType, formatInfo.Converter ? " (c)" : ""
	);
	g_irrlogger->log(lbuf, ELL_DEBUG);

	const auto *tmpImages = &srcImages;

	if (KeepImage || OriginalSize != Size || OriginalColorFormat != ColorFormat) {
		Images.resize(srcImages.size());

		for (size_t i = 0; i < srcImages.size(); ++i) {
			Images[i] = Driver->createImage(ColorFormat, Size);

			if (srcImages[i]->getDimension() == Size)
				srcImages[i]->copyTo(Images[i]);
			else
				srcImages[i]->copyToScaling(Images[i]);
		}

		tmpImages = &Images;
	}

#ifdef _IRR_COMPILE_WITH_OPENGL_
	glGenTextures(1, &TextureName);
	Driver->testGLError();
#endif
	if (!TextureName) {
		g_irrlogger->log("GLTexture: texture not created", ELL_ERROR);
		return;
	}

	auto ctxt = Driver->getContext();
	auto prevTexture = static_cast<const GLTexture *>(ctxt->getTextureUnit(0));
	ctxt->setTextureUnit(0, this);

#ifdef _IRR_COMPILE_WITH_OPENGL_
	glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (HasMipMaps) {
		if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
			glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
		else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
			glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		else
			glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);
	}
	Driver->testGLError();
#endif

	initTexture(tmpImages->size());

	for (size_t i = 0; i < tmpImages->size(); ++i)
		uploadTexture(i, 0, (*tmpImages)[i]->getData());

	if (HasMipMaps) {
		for (size_t i = 0; i < tmpImages->size(); ++i)
			regenerateMipMapLevels(i);
	}

	if (!KeepImage) {
		for (size_t i = 0; i < Images.size(); ++i)
			Images[i]->drop();

		Images.clear();
	}

	if (!name.empty())
		Driver->GLInfo->ObjectLabel(GL_TEXTURE, TextureName, name.c_str());

	ctxt->setTextureUnit(0, prevTexture);

	Driver->testGLError();
}

GLTexture::GLTexture(const io::path &name, const core::dimension2d<uint32_t> &size,
                     E_TEXTURE_TYPE type, ECOLOR_FORMAT format, VideoDriver *driver,
                     uint8_t msaa) :
	NamedPath(name),
	DriverType(driver->getDriverType()),
	OriginalSize(size),
	Size(size),
	OriginalColorFormat(format),
	ColorFormat(ECF_UNKNOWN),
	Pitch(0),
	HasMipMaps(false),
	IsRenderTarget(true),
	Type(type),
	Driver(driver),
	TextureType(GL_TEXTURE_2D),
	TextureName(0),
	InternalFormat(GL_RGBA),
	PixelFormat(GL_RGBA),
	PixelType(GL_UNSIGNED_BYTE),
	MSAA(msaa),
	Converter(nullptr),
	LockReadOnly(false),
	LockImage(nullptr),
	LockLayer(0),
	KeepImage(false),
	MipLevelStored(0)
{
	DriverType = Driver->getDriverType();
	assert(Type != ETT_2D_ARRAY);
	TextureType = textureTypeIrrToGL(Type);
	HasMipMaps = false;
	IsRenderTarget = true;

	OriginalColorFormat = format;

	if (ECF_UNKNOWN == OriginalColorFormat)
		ColorFormat = getBestColorFormat(Driver->getColorFormat());
	else
		ColorFormat = OriginalColorFormat;

	OriginalSize = size;
	Size = OriginalSize;

	Pitch = Size.Width * pixelFormatsInfo[ColorFormat].size / 8;

	InternalFormat = GLSpecificInfo::TextureFormats[ColorFormat].InternalFormat;
	PixelFormat = GLSpecificInfo::TextureFormats[ColorFormat].PixelFormat;
	PixelType = GLSpecificInfo::TextureFormats[ColorFormat].PixelType;
	Converter = GLSpecificInfo::TextureFormats[ColorFormat].Converter;
	if (InternalFormat == 0) {
		g_irrlogger->log("GLTexture: Color format is not supported",
			pixelFormatsInfo[ColorFormat < ECF_UNKNOWN ? ColorFormat : ECF_UNKNOWN].name.c_str(), ELL_ERROR);
		return;
	}

	// On GLES 3.0 we must use sized internal formats
	if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3) {
		InternalFormat = GL_RGBA8;
		PixelFormat = GL_RGBA;
	}

	char lbuf[100];
	snprintf_irr(lbuf, sizeof(lbuf),
		"GLTexture: RTT Type = %d Size = %dx%d ColorFormat = %d -> %#06x %#06x %#06x%s",
		(int)Type, Size.Width, Size.Height, (int)ColorFormat,
		InternalFormat, PixelFormat, PixelType, Converter ? " (c)" : ""
	);
	g_irrlogger->log(lbuf, ELL_DEBUG);

#ifdef _IRR_COMPILE_WITH_OPENGL_
	glGenTextures(1, &TextureName);
	Driver->testGLError();
#endif
	if (!TextureName) {
		g_irrlogger->log("GLTexture: texture not created", ELL_ERROR);
		return;
	}

	auto ctxt = Driver->getContext();
	auto prevTexture = static_cast<const GLTexture *>(ctxt->getTextureUnit(0));
	ctxt->setTextureUnit(0, this);

	if (Type != ETT_2D_MS) {
#ifdef _IRR_COMPILE_WITH_OPENGL_
		glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(TextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif
		StatesCache.WrapU = ETC_CLAMP_TO_EDGE;
		StatesCache.WrapV = ETC_CLAMP_TO_EDGE;
		StatesCache.WrapW = ETC_CLAMP_TO_EDGE;
	}

	initTexture(0);

	if (!name.empty())
		Driver->GLInfo->ObjectLabel(GL_TEXTURE, TextureName, name.c_str());

	ctxt->setTextureUnit(0, prevTexture);
	Driver->testGLError();
}

GLTexture::~GLTexture()
{
#ifdef _IRR_COMPILE_WITH_OPENGL_
	if (TextureName)
		glDeleteTextures(1, &TextureName);
#endif

	if (LockImage)
		LockImage->drop();

	for (auto *image : Images)
		image->drop();
}

//=============================================================================
// ITexture implementation
//=============================================================================

void *GLTexture::lock(E_TEXTURE_LOCK_MODE mode, uint32_t mipmapLevel,
                       uint32_t layer, E_TEXTURE_LOCK_FLAGS lockFlags)
{
	if (LockImage)
		return LockImage->getData();

	LockReadOnly |= (mode == ETLM_READ_ONLY);
	LockLayer = layer;
	MipLevelStored = mipmapLevel;

	if (KeepImage) {
		assert(LockLayer < Images.size());

		if (mipmapLevel == 0) {
			LockImage = Images[LockLayer];
			LockImage->grab();
		}
	}

	if (!LockImage) {
		core::dimension2d<uint32_t> lockImageSize(getMipMapsSize(Size, MipLevelStored));
		assert(lockImageSize.Width > 0 && lockImageSize.Height > 0);

		LockImage = Driver->createImage(ColorFormat, lockImageSize);

		if (LockImage && mode != ETLM_WRITE_ONLY) {
			bool passed = true;

			const bool use_gl_impl = Driver->getVersion().Spec != OpenGLSpec::ES;

			auto ctxt = Driver->getContext();

			if (Type == ETT_2D_ARRAY) {
				g_irrlogger->log("lock: read or read/write unimplemented for ETT_2D_ARRAY", ELL_WARNING);
				passed = false;

			} else if (use_gl_impl) {
#ifdef _IRR_COMPILE_WITH_OPENGL3_
				IImage *tmpImage = LockImage;

				ctxt->setTextureUnit(0, this);
				Driver->testGLError();

				uint32_t tmpTextureType = getTextureTarget(layer);

				glGetTexImage(tmpTextureType, MipLevelStored, PixelFormat, PixelType, tmpImage->getData());
				Driver->testGLError();

				if (IsRenderTarget && lockFlags == ETLF_FLIP_Y_UP_RTT)
					flipImageY(tmpImage);
#endif
			} else {
				auto tmpFBO = new RenderTarget(Driver);
				auto prevFBO = ctxt->getRenderTarget();
				ctxt->setRenderTarget(tmpFBO);

				tmpFBO->setColorTextures({this}, {}, mipmapLevel);

				IImage *tmpImage = Driver->createImage(ECF_A8R8G8B8, lockImageSize);
#ifdef _IRR_COMPILE_WITH_OPENGL_
				glReadPixels(0, 0, lockImageSize.Width, lockImageSize.Height,
					GL_RGBA, GL_UNSIGNED_BYTE, tmpImage->getData());
#endif
				tmpFBO->setColorTextures({}, {});

				ctxt->setRenderTarget(prevFBO);

				tmpFBO->drop();

				if (IsRenderTarget && lockFlags == ETLF_FLIP_Y_UP_RTT)
					flipImageY(tmpImage);

				void *src = tmpImage->getData();
				void *dest = LockImage->getData();

				switch (ColorFormat) {
				case ECF_A1R5G5B5:
					CColorConverter::convert_A8R8G8B8toA1B5G5R5(src, tmpImage->getDimension().getArea(), dest);
					break;
				case ECF_R5G6B5:
					CColorConverter::convert_A8R8G8B8toR5G6B5(src, tmpImage->getDimension().getArea(), dest);
					break;
				case ECF_R8G8B8:
					CColorConverter::convert_A8R8G8B8toB8G8R8(src, tmpImage->getDimension().getArea(), dest);
					break;
				case ECF_A8R8G8B8:
					CColorConverter::convert_A8R8G8B8toA8B8G8R8(src, tmpImage->getDimension().getArea(), dest);
					break;
				default:
					passed = false;
					break;
				}
				tmpImage->drop();
			}

			if (!passed) {
				LockImage->drop();
				LockImage = nullptr;
			}
		}
	}

	return (LockImage) ? LockImage->getData() : nullptr;
}

void GLTexture::unlock()
{
	if (!LockImage)
		return;

	if (!LockReadOnly) {
		auto prevTexture = static_cast<const GLTexture *>(Driver->getContext()->getTextureUnit(0));
		Driver->getContext()->setTextureUnit(0, this);

		uploadTexture(LockLayer, MipLevelStored, LockImage->getData());

		Driver->getContext()->setTextureUnit(0, prevTexture);
	}

	LockImage->drop();

	LockReadOnly = false;
	LockImage = nullptr;
	LockLayer = 0;
}

void GLTexture::regenerateMipMapLevels(uint32_t layer)
{
	if (!HasMipMaps || (Size.Width <= 1 && Size.Height <= 1))
		return;

	auto prevTexture = static_cast<const GLTexture *>(Driver->getContext()->getTextureUnit(0));
	Driver->getContext()->setTextureUnit(0, this);

#ifdef _IRR_COMPILE_WITH_OPENGL_
	glGenerateMipmap(TextureType);
	Driver->testGLError();
#endif

	Driver->getContext()->setTextureUnit(0, prevTexture);
}

uint32_t GLTexture::getID() const
{
	return TextureName;
}

bool GLTexture::hasAlpha() const
{
	switch (ColorFormat) {
	case ECF_A8R8G8B8:
	case ECF_A1R5G5B5:
	case ECF_A16B16G16R16F:
	case ECF_A32B32G32R32F:
		return true;
	default:
		return false;
	}
}

//=============================================================================
// Protected methods
//=============================================================================

core::dimension2d<uint32_t> GLTexture::getMipMapsSize(const core::dimension2d<uint32_t> &baseSize,
                                                       uint32_t mipLevel)
{
	uint32_t w = baseSize.Width >> mipLevel;
	uint32_t h = baseSize.Height >> mipLevel;
	return core::dimension2d<uint32_t>(w > 0 ? w : 1, h > 0 ? h : 1);
}

ECOLOR_FORMAT GLTexture::getBestColorFormat(ECOLOR_FORMAT format)
{
	ECOLOR_FORMAT destFormat = (format <= ECF_A8R8G8B8) ? ECF_A8R8G8B8 : format;

	switch (format) {
	case ECF_A1R5G5B5:
		if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
			destFormat = ECF_A1R5G5B5;
		break;
	case ECF_R5G6B5:
		if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
			destFormat = ECF_R5G6B5;
		break;
	case ECF_A8R8G8B8:
		if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
		    Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
			destFormat = ECF_A1R5G5B5;
		break;
	case ECF_R8G8B8:
		if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
		    Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
			destFormat = ECF_A1R5G5B5;
	default:
		break;
	}

	if (Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL)) {
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

void GLTexture::getImageValues(const IImage *image)
{
	OriginalColorFormat = image->getColorFormat();
	ColorFormat = getBestColorFormat(OriginalColorFormat);

	InternalFormat = GLSpecificInfo::TextureFormats[ColorFormat].InternalFormat;
	PixelFormat = GLSpecificInfo::TextureFormats[ColorFormat].PixelFormat;
	PixelType = GLSpecificInfo::TextureFormats[ColorFormat].PixelType;
	Converter = GLSpecificInfo::TextureFormats[ColorFormat].Converter;
	if (InternalFormat == 0) {
		g_irrlogger->log("getImageValues: Color format is not supported",
			pixelFormatsInfo[ColorFormat < ECF_UNKNOWN ? ColorFormat : ECF_UNKNOWN].name.c_str(), ELL_ERROR);
		InternalFormat = 0;
		return;
	}

	OriginalSize = image->getDimension();
	Size = OriginalSize;

	if (Size.Width == 0 || Size.Height == 0) {
		g_irrlogger->log("Invalid size of image for texture.", ELL_ERROR);
		return;
	}

	const float ratio = (float)Size.Width / (float)Size.Height;

	auto features = Driver->getFeatures();
	if ((Size.Width > features.MaxTextureSize) && (ratio >= 1.f)) {
		Size.Width = features.MaxTextureSize;
		Size.Height = (uint32_t)(features.MaxTextureSize / ratio);
	} else if (Size.Height > features.MaxTextureSize) {
		Size.Height = features.MaxTextureSize;
		Size.Width = (uint32_t)(features.MaxTextureSize * ratio);
	}

	bool needSquare = (Type == ETT_CUBEMAP);
	Size = Size.getOptimalSize(false, needSquare, true, features.MaxTextureSize);

	Pitch = Size.Width * pixelFormatsInfo[ColorFormat].size / 8;
}

void GLTexture::flipImageY(IImage *image)
{
	const uint32_t pitch = image->getPitch();
	uint8_t *srcA = static_cast<uint8_t *>(image->getData());
	uint8_t *srcB = srcA + (image->getDimension().Height - 1) * pitch;

	std::vector<uint8_t> tmpBuffer(pitch);
	for (uint32_t i = 0; i < image->getDimension().Height; i += 2) {
		memcpy(tmpBuffer.data(), srcA, pitch);
		memcpy(srcA, srcB, pitch);
		memcpy(srcB, tmpBuffer.data(), pitch);
		srcA += pitch;
		srcB -= pitch;
	}
}

void GLTexture::initTexture(uint32_t layers)
{
	uint32_t levels = 1;
	if (HasMipMaps) {
		levels = core::u32_log2(core::max_(Size.Width, Size.Height)) + 1;
	}

	bool use_tex_storage = Driver->getFeatures().TexStorage;

	if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3)
		use_tex_storage = false;

#ifdef _IRR_COMPILE_WITH_OPENGL_
	switch (Type) {
	case ETT_2D:
		if (use_tex_storage) {
			glTexStorage2D(TextureType, levels, InternalFormat,
				Size.Width, Size.Height);
		} else {
			glTexImage2D(TextureType, 0, InternalFormat,
				Size.Width, Size.Height, 0, PixelFormat, PixelType, nullptr);
		}
		Driver->testGLError();
		break;
	case ETT_2D_MS: {
		GLint max_samples = 0;
		glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
		MSAA = core::min_(MSAA, (uint8_t)max_samples);

		const bool use_gl_impl = Driver->getVersion().Spec != OpenGLSpec::ES;

		if (use_gl_impl)
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, InternalFormat,
			                         Size.Width, Size.Height, GL_TRUE);
		else
			glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, InternalFormat,
			                           Size.Width, Size.Height, GL_TRUE);
		Driver->testGLError();
		break;
	}
	case ETT_CUBEMAP:
		for (uint32_t i = 0; i < 6; i++) {
			uint32_t target = getTextureTarget(i);
			if (use_tex_storage) {
				glTexStorage2D(target, levels, InternalFormat,
					Size.Width, Size.Height);
			} else {
				glTexImage2D(target, 0, InternalFormat,
					Size.Width, Size.Height, 0, PixelFormat, PixelType, nullptr);
			}
			Driver->testGLError();
		}
		break;
	case ETT_2D_ARRAY:
		if (use_tex_storage) {
			glTexStorage3D(TextureType, levels, InternalFormat,
				Size.Width, Size.Height, layers);
		} else {
			glTexImage3D(TextureType, 0, InternalFormat,
				Size.Width, Size.Height, layers, 0, PixelFormat, PixelType, nullptr);
		}
		Driver->testGLError();
		break;
	default:
		assert(false);
		break;
	}
#endif
}

void GLTexture::uploadTexture(uint32_t layer, uint32_t level, void *data)
{
	if (!data)
		return;

	uint32_t width = Size.Width >> level;
	uint32_t height = Size.Height >> level;
	if (width < 1) width = 1;
	if (height < 1) height = 1;

	uint32_t tmpTextureType = getTextureTarget(layer);

	CImage *tmpImage = nullptr;
	void *tmpData = data;

	if (Converter) {
		const core::dimension2d<uint32_t> tmpImageSize(width, height);
		tmpImage = new CImage(ColorFormat, tmpImageSize);
		tmpData = tmpImage->getData();
		Converter(data, tmpImageSize.getArea(), tmpData);
	}

#ifdef _IRR_COMPILE_WITH_OPENGL_
	switch (TextureType) {
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP:
		glTexSubImage2D(tmpTextureType, level, 0, 0, width, height,
		                PixelFormat, PixelType, tmpData);
		break;
	case GL_TEXTURE_2D_ARRAY:
		glTexSubImage3D(tmpTextureType, level, 0, 0, layer, width, height, 1,
		                PixelFormat, PixelType, tmpData);
		break;
	default:
		assert(false);
		break;
	}
	Driver->testGLError();
#endif

	delete tmpImage;
}

uint32_t GLTexture::getTextureTarget(uint32_t layer) const
{
	uint32_t tmp = TextureType;
	if (tmp == GL_TEXTURE_CUBE_MAP) {
		assert(layer < 6);
		tmp = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
	}
	return tmp;
}

uint32_t GLTexture::textureTypeIrrToGL(E_TEXTURE_TYPE type) const
{
	switch (type) {
	case ETT_2D:        return GL_TEXTURE_2D;
	case ETT_2D_MS:     return GL_TEXTURE_2D_MULTISAMPLE;
	case ETT_CUBEMAP:   return GL_TEXTURE_CUBE_MAP;
	case ETT_2D_ARRAY:  return GL_TEXTURE_2D_ARRAY;
	}

	g_irrlogger->log("GLTexture::textureTypeIrrToGL unknown texture type", ELL_WARNING);
	return GL_TEXTURE_2D;
}

} // end namespace video
