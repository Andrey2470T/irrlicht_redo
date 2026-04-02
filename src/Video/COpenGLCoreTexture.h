// Copyright (C) 2015 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <vector>
#include <cassert>

#include "SMaterialLayer.h"
#include "ITexture.h"
#include "Logger.h"
#include "Image/CImage.h"
#include "Image/CColorConverter.h"
#include "VideoDriver.h"
#include "GLSpecificInfo.h"


namespace video
{

class COpenGLCoreTexture : public ITexture
{
public:
	struct SStatesCache
	{
		SStatesCache() :
				WrapU(ETC_REPEAT), WrapV(ETC_REPEAT), WrapW(ETC_REPEAT),
				LODBias(0), AnisotropicFilter(0), MinFilter(video::ETMINF_NEAREST_MIPMAP_NEAREST),
				MagFilter(video::ETMAGF_NEAREST), MipMapStatus(false), IsCached(false)
		{
		}

		u8 WrapU;
		u8 WrapV;
		u8 WrapW;
		s8 LODBias;
		u8 AnisotropicFilter;
		video::E_TEXTURE_MIN_FILTER MinFilter;
		video::E_TEXTURE_MAG_FILTER MagFilter;
		bool MipMapStatus;
		bool IsCached;
	};

	COpenGLCoreTexture(const io::path &name, const std::vector<IImage *> &srcImages, E_TEXTURE_TYPE type, VideoDriver *driver) :
			ITexture(name, type), Driver(driver), TextureType(GL_TEXTURE_2D),
			TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA), PixelType(GL_UNSIGNED_BYTE), MSAA(0), Converter(0), LockReadOnly(false), LockImage(0), LockLayer(0),
			KeepImage(false), MipLevelStored(0)
	{
		assert(!srcImages.empty());

		DriverType = Driver->getDriverType();
		assert(Type != ETT_2D_MS); // not supported by this constructor
		TextureType = TextureTypeIrrToGL(Type);
		HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
		KeepImage = Driver->getTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY);

		getImageValues(srcImages[0]);
		if (!InternalFormat)
			return;

		char lbuf[128];
		snprintf_irr(lbuf, sizeof(lbuf),
			"COpenGLCoreTexture: Type = %d Size = %dx%d (%dx%d) ColorFormat = %d (%d)%s -> %#06x %#06x %#06x%s",
			(int)Type, Size.Width, Size.Height, OriginalSize.Width, OriginalSize.Height,
			(int)ColorFormat, (int)OriginalColorFormat,
			HasMipMaps ? " +Mip" : "",
			InternalFormat, PixelFormat, PixelType, Converter ? " (c)" : ""
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

		glGenTextures(1, &TextureName);
		Driver->testGLError();
		if (!TextureName) {
			g_irrlogger->log("COpenGLCoreTexture: texture not created", ELL_ERROR);
			return;
		}

		auto ctxt = Driver->getContext();
		auto prevTexture = static_cast<const COpenGLCoreTexture *>(ctxt->getTextureUnit(0));
		ctxt->setTextureUnit(0, this);

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

	COpenGLCoreTexture(const io::path &name, const core::dimension2d<u32> &size, E_TEXTURE_TYPE type, ECOLOR_FORMAT format, VideoDriver *driver, u8 msaa = 0) :
			ITexture(name, type),
			Driver(driver), TextureType(GL_TEXTURE_2D),
			TextureName(0), InternalFormat(GL_RGBA), PixelFormat(GL_RGBA), PixelType(GL_UNSIGNED_BYTE), MSAA(msaa), Converter(0), LockReadOnly(false), LockImage(0), LockLayer(0), KeepImage(false),
			MipLevelStored(0)
	{
		DriverType = Driver->getDriverType();
		assert(Type != ETT_2D_ARRAY); // not supported by this constructor
		TextureType = TextureTypeIrrToGL(Type);
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
			g_irrlogger->log("COpenGLCoreTexture: Color format is not supported",
				pixelFormatsInfo[ColorFormat < ECF_UNKNOWN ? ColorFormat : ECF_UNKNOWN].name.c_str(), ELL_ERROR);
			return;
		}

		// On GLES 3.0 we must use sized internal formats for textures when calling
		// glTexStorage. But ECF_A8R8G8B8 might be mapped to GL_BGRA (an unsized format).
		// Since we don't upload to RTT we can safely pick a different combo that works.
		if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3) {
			InternalFormat = GL_RGBA8;
			PixelFormat = GL_RGBA;
		}

		char lbuf[100];
		snprintf_irr(lbuf, sizeof(lbuf),
			"COpenGLCoreTexture: RTT Type = %d Size = %dx%d ColorFormat = %d -> %#06x %#06x %#06x%s",
			(int)Type, Size.Width, Size.Height, (int)ColorFormat,
			InternalFormat, PixelFormat, PixelType, Converter ? " (c)" : ""
		);
		g_irrlogger->log(lbuf, ELL_DEBUG);

		glGenTextures(1, &TextureName);
		Driver->testGLError();
		if (!TextureName) {
			g_irrlogger->log("COpenGLCoreTexture: texture not created", ELL_ERROR);
			return;
		}

		auto ctxt = Driver->getContext();
		auto prevTexture = static_cast<const COpenGLCoreTexture *>(ctxt->getTextureUnit(0));
		ctxt->setTextureUnit(0, this);

		// An INVALID_ENUM error is generated by TexParameter* if target is either
		// TEXTURE_2D_MULTISAMPLE or TEXTURE_2D_MULTISAMPLE_ARRAY, and pname is any
		// sampler state from table 23.18.
		// ~ https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf
		if (Type != ETT_2D_MS) {
			glTexParameteri(TextureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(TextureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(TextureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(TextureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(TextureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

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

	virtual ~COpenGLCoreTexture()
	{
		if (TextureName)
			glDeleteTextures(1, &TextureName);

		if (LockImage)
			LockImage->drop();

		for (auto *image : Images)
			image->drop();
	}

	void *lock(E_TEXTURE_LOCK_MODE mode = ETLM_READ_WRITE, u32 mipmapLevel = 0, u32 layer = 0, E_TEXTURE_LOCK_FLAGS lockFlags = ETLF_FLIP_Y_UP_RTT) override
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
			core::dimension2d<u32> lockImageSize(IImage::getMipMapsSize(Size, MipLevelStored));
			assert(lockImageSize.Width > 0 && lockImageSize.Height > 0);

			LockImage = Driver->createImage(ColorFormat, lockImageSize);

			if (LockImage && mode != ETLM_WRITE_ONLY) {
				bool passed = true;

				const bool use_gl_impl = Driver->getVersion().Spec != OpenGLSpec::ES;

				auto ctxt = Driver->getContext();

				if (Type == ETT_2D_ARRAY) {

				// For OpenGL an array texture is basically just a 3D texture internally.
				// So if we call glGetTexImage() we would download the entire array,
				// except the caller only wants a single layer.
				// To do this properly we could use glGetTextureSubImage() [4.5]
				// or some trickery with glTextureView() [4.3].
				// Also neither of those will work on GLES.

				g_irrlogger->log("lock: read or read/write unimplemented for ETT_2D_ARRAY", ELL_WARNING);
				passed = false;

				} else if (use_gl_impl) {

#ifdef _IRR_COMPILE_WITH_OPENGL3_
				IImage *tmpImage = LockImage;

				ctxt->setTextureUnit(0, this);
				Driver->testGLError();

				GLenum tmpTextureType = getTextureTarget(layer);

				glGetTexImage(tmpTextureType, MipLevelStored, PixelFormat, PixelType, tmpImage->getData());
				Driver->testGLError();

				if (IsRenderTarget && lockFlags == ETLF_FLIP_Y_UP_RTT)
					flipImageY(tmpImage);
#endif

				} else {

				auto tmpFBO = new RenderTarget(Driver);
				auto prevFBO = ctxt->getRenderTarget();
				ctxt->setRenderTarget(tmpFBO);

				// Warning: on GLES 2.0 this call will only work with mipmapLevel == 0
				tmpFBO->setColorTextures({this}, {}, mipmapLevel);

				IImage *tmpImage = Driver->createImage(ECF_A8R8G8B8, lockImageSize);
				glReadPixels(0, 0, lockImageSize.Width, lockImageSize.Height,
					GL_RGBA, GL_UNSIGNED_BYTE, tmpImage->getData());

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
					LockImage = 0;
				}
			}
		}

		return (LockImage) ? LockImage->getData() : 0;
	}

	void unlock() override
	{
		if (!LockImage)
			return;

		if (!LockReadOnly) {
			auto prevTexture = static_cast<const COpenGLCoreTexture *>(Driver->getContext()->getTextureUnit(0));
			Driver->getContext()->setTextureUnit(0, this);

			uploadTexture(LockLayer, MipLevelStored, LockImage->getData());

			Driver->getContext()->setTextureUnit(0, prevTexture);
		}

		LockImage->drop();

		LockReadOnly = false;
		LockImage = 0;
		LockLayer = 0;
	}

	void regenerateMipMapLevels(u32 layer = 0) override
	{
		if (!HasMipMaps || (Size.Width <= 1 && Size.Height <= 1))
			return;

		auto prevTexture = static_cast<const COpenGLCoreTexture *>(Driver->getContext()->getTextureUnit(0));
		Driver->getContext()->setTextureUnit(0, this);

		glGenerateMipmap(TextureType);
		Driver->testGLError();

		Driver->getContext()->setTextureUnit(0, prevTexture);
	}

	GLenum getOpenGLTextureType() const
	{
		return TextureType;
	}

	u32 getID() const override
	{
		return (u32)TextureName;
	}

	SStatesCache &getStatesCache() const
	{
		return StatesCache;
	}

protected:
	ECOLOR_FORMAT getBestColorFormat(ECOLOR_FORMAT format)
	{
		// We only try for to adapt "simple" formats
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
			// Note: Using ECF_A8R8G8B8 even when ETCF_ALWAYS_32_BIT is not set as 24 bit textures fail with too many cards
			if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) || Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
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

	void getImageValues(const IImage *image)
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

		const f32 ratio = (f32)Size.Width / (f32)Size.Height;

		auto features = Driver->getFeatures();
		if ((Size.Width > features.MaxTextureSize) && (ratio >= 1.f)) {
			Size.Width = features.MaxTextureSize;
			Size.Height = (u32)(features.MaxTextureSize / ratio);
		} else if (Size.Height > features.MaxTextureSize) {
			Size.Height = features.MaxTextureSize;
			Size.Width = (u32)(features.MaxTextureSize * ratio);
		}

		bool needSquare = (Type == ETT_CUBEMAP);

		Size = Size.getOptimalSize(false, needSquare, true, features.MaxTextureSize);

		Pitch = Size.Width * pixelFormatsInfo[ColorFormat].size / 8;
	}

	static void flipImageY(IImage *image)
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

	void initTexture(u32 layers)
	{
		u32 levels = 1;
		if (HasMipMaps) {
			levels = core::u32_log2(core::max_(Size.Width, Size.Height)) + 1;
		}

		// reference: <https://www.khronos.org/opengl/wiki/Texture_Storage>
		bool use_tex_storage = Driver->getFeatures().TexStorage;

		// On GLES 3.0 if we don't have a sized format suitable for glTexStorage,
		// just avoid using it. Only affects the extension that provides BGRA.
		if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3)
			use_tex_storage = false;

		switch (Type) {
		case ETT_2D:
			if (use_tex_storage) {
				glTexStorage2D(TextureType, levels, InternalFormat,
					Size.Width, Size.Height);
			} else {
				glTexImage2D(TextureType, 0, InternalFormat,
					Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
			}
			Driver->testGLError();
			break;
		case ETT_2D_MS: {
			GLint max_samples = 0;
			glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
			MSAA = core::min_(MSAA, (u8)max_samples);

			// glTexImage2DMultisample is supported by OpenGL 3.2+
			// glTexStorage2DMultisample is supported by OpenGL 4.3+ and OpenGL ES 3.1+
			// so pick the most compatible one
			const bool use_gl_impl = Driver->getVersion().Spec != OpenGLSpec::ES;

			if (use_gl_impl)
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, InternalFormat, Size.Width, Size.Height, GL_TRUE);
			else
				glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, InternalFormat, Size.Width, Size.Height, GL_TRUE);
			Driver->testGLError();
			break;
		}
		case ETT_CUBEMAP:
			for (u32 i = 0; i < 6; i++) {
				GLenum target = getTextureTarget(i);
				if (use_tex_storage) {
					glTexStorage2D(target, levels, InternalFormat,
						Size.Width, Size.Height);
				} else {
					glTexImage2D(target, 0, InternalFormat,
						Size.Width, Size.Height, 0, PixelFormat, PixelType, 0);
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
					Size.Width, Size.Height, layers, 0, PixelFormat, PixelType, 0);
			}
			Driver->testGLError();
			break;
		default:
			assert(false);
			break;
		}
	}

	void uploadTexture(u32 layer, u32 level, void *data)
	{
		if (!data)
			return;

		u32 width = Size.Width >> level;
		u32 height = Size.Height >> level;
		if (width < 1)
			width = 1;
		if (height < 1)
			height = 1;

		GLenum tmpTextureType = getTextureTarget(layer);

		CImage *tmpImage = 0;
		void *tmpData = data;

		if (Converter) {
			const core::dimension2d<u32> tmpImageSize(width, height);

			tmpImage = new CImage(ColorFormat, tmpImageSize);
			tmpData = tmpImage->getData();

			Converter(data, tmpImageSize.getArea(), tmpData);
		}

		switch (TextureType) {
		case GL_TEXTURE_2D:
		case GL_TEXTURE_CUBE_MAP:
			glTexSubImage2D(tmpTextureType, level, 0, 0, width, height, PixelFormat, PixelType, tmpData);
			break;
		case GL_TEXTURE_2D_ARRAY:
			glTexSubImage3D(tmpTextureType, level, 0, 0, layer, width, height, 1, PixelFormat, PixelType, tmpData);
			break;
		default:
			assert(false);
			break;
		}
		Driver->testGLError();

		delete tmpImage;
	}

	GLenum TextureTypeIrrToGL(E_TEXTURE_TYPE type) const
	{
		switch (type) {
		case ETT_2D:
			return GL_TEXTURE_2D;
		case ETT_2D_MS:
			return GL_TEXTURE_2D_MULTISAMPLE;
		case ETT_CUBEMAP:
			return GL_TEXTURE_CUBE_MAP;
		case ETT_2D_ARRAY:
			return GL_TEXTURE_2D_ARRAY;
		}

		g_irrlogger->log("COpenGLCoreTexture::TextureTypeIrrToGL unknown texture type", ELL_WARNING);
		return GL_TEXTURE_2D;
	}

	GLenum getTextureTarget(u32 layer) const
	{
		GLenum tmp = TextureType;
		if (tmp == GL_TEXTURE_CUBE_MAP) {
			assert(layer < 6);
			tmp = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
		}
		return tmp;
	}

	VideoDriver *Driver;

	GLenum TextureType;
	GLuint TextureName;
	GLint InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;
	u8 MSAA;
	void (*Converter)(const void *, s32, void *);

	bool LockReadOnly;
	IImage *LockImage;
	u32 LockLayer;

	bool KeepImage;
	std::vector<IImage*> Images;

	u8 MipLevelStored;

	mutable SStatesCache StatesCache;
};

}
