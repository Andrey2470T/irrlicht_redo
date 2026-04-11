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

std::array<GLenum, ETT_COUNT> toGLTexType = {
	GL_TEXTURE_2D,
	GL_TEXTURE_2D_MULTISAMPLE,
	GL_TEXTURE_CUBE_MAP
};

GLenum getTextureTarget(E_TEXTURE_TYPE type, u32 layer)
{
	GLenum tmp = toGLTexType[type];
	if (tmp == GL_TEXTURE_CUBE_MAP) {
		assert(layer < 6);
		tmp = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
	}
	return tmp;
}

GLTexture::GLTexture(const io::path &name, const std::vector<IImage *> &srcImages,
                     E_TEXTURE_TYPE type, VideoDriver *driver, const TextureSettings &settings) :
    NamedPath(name), Type(type), Driver(driver), TexSettings(settings)
{
	assert(!srcImages.empty() || Type != ETT_2D_MS);

    TexSettings.HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS) || TexSettings.HasMipMaps;
	KeepImage = Driver->getTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY);

	getImageValues(srcImages[0]);

	auto &formatInfo = GLSpecificInfo::TextureFormats[ColorFormat];

	char lbuf[128];
	snprintf_irr(lbuf, sizeof(lbuf),
		"GLTexture: Type = %d Size = %dx%d (%dx%d) ColorFormat = %d (%d)%s -> %#06x %#06x %#06x%s",
		(int)Type, Size.Width, Size.Height, OriginalSize.Width, OriginalSize.Height,
		(int)ColorFormat, (int)OriginalColorFormat,
        TexSettings.HasMipMaps ? " +Mip" : "",
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

    genTexture();

	auto ctxt = Driver->getContext();
	auto prevTexture = ctxt->getTextureUnit(0);
	ctxt->setTextureUnit(0, this);

    initTexture();

	for (size_t i = 0; i < tmpImages->size(); ++i)
		uploadTexture(i, 0, (*tmpImages)[i]->getData());

    if (TexSettings.HasMipMaps) {
        regenerateMipMaps();
    }

	if (!KeepImage) {
		for (size_t i = 0; i < Images.size(); ++i)
			Images[i]->drop();

		Images.clear();
	}

	ctxt->setTextureUnit(0, prevTexture);
}

GLTexture::GLTexture(const io::path &name, const core::dimension2du &size,
                     E_TEXTURE_TYPE type, ECOLOR_FORMAT format, VideoDriver *driver,
                     u8 msaa) :
    NamedPath(name), OriginalSize(size), Size(size), OriginalColorFormat(format),
    ColorFormat(format), Type(type), Driver(driver), MSAA(msaa)
{
    TexSettings.IsRenderTarget = true;

    if (OriginalColorFormat == ECF_UNKNOWN)
		ColorFormat = getBestColorFormat(Driver->getColorFormat());

	Pitch = Size.Width * pixelFormatsInfo[ColorFormat].size / 8;

	// On GLES 3.0 we must use sized internal formats
    //if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3) {
    //	InternalFormat = GL_RGBA8;
    //	PixelFormat = GL_RGBA;
    //}

    auto &formatInfo = GLSpecificInfo::TextureFormats[ColorFormat];

	char lbuf[100];
	snprintf_irr(lbuf, sizeof(lbuf),
		"GLTexture: RTT Type = %d Size = %dx%d ColorFormat = %d -> %#06x %#06x %#06x%s",
		(int)Type, Size.Width, Size.Height, (int)ColorFormat,
        formatInfo.InternalFormat, formatInfo.PixelFormat, formatInfo.PixelType, formatInfo.Converter ? " (c)" : ""
	);
	g_irrlogger->log(lbuf, ELL_DEBUG);

    genTexture();

	auto ctxt = Driver->getContext();
    auto prevTexture = ctxt->getTextureUnit(0);
	ctxt->setTextureUnit(0, this);

    initTexture();

	ctxt->setTextureUnit(0, prevTexture);
}

GLTexture::~GLTexture()
{
    if (TexID) glDeleteTextures(1, &TexID);

	if (LockImage)
		LockImage->drop();

	for (auto *image : Images)
		image->drop();
}

void GLTexture::bind() const
{
    glBindTexture(toGLTexType[Type], TexID);
    TEST_GL_ERROR(Driver);
}

void GLTexture::unbind() const
{
    glBindTexture(toGLTexType[Type], 0);
    TEST_GL_ERROR(Driver);
}

void *GLTexture::lock(E_TEXTURE_LOCK_MODE mode, u32 mipLevel, u32 layer)
{
	if (LockImage)
		return LockImage->getData();

	LockReadOnly |= (mode == ETLM_READ_ONLY);
	LockLayer = layer;
    LockMipLevel = mipLevel;

	if (KeepImage) {
		assert(LockLayer < Images.size());

        if (mipLevel == 0) {
			LockImage = Images[LockLayer];
			LockImage->grab();
		}
	}

	if (!LockImage) {
        core::dimension2du lockImageSize(getMipMapsSize(LockMipLevel));
		assert(lockImageSize.Width > 0 && lockImageSize.Height > 0);

		LockImage = Driver->createImage(ColorFormat, lockImageSize);

		if (LockImage && mode != ETLM_WRITE_ONLY) {
			auto ctxt = Driver->getContext();

#ifdef _IRR_COMPILE_WITH_OPENGL3_
            IImage *tmpImage = LockImage;

            ctxt->setTextureUnit(0, this);
            TEST_GL_ERROR(Driver);

            auto &formatInfo = GLSpecificInfo::TextureFormats[ColorFormat];

            glGetTexImage(getTextureTarget(Type, layer), LockMipLevel, formatInfo.PixelFormat, formatInfo.PixelType, tmpImage->getData());
            TEST_GL_ERROR(Driver);

            if (TexSettings.IsRenderTarget)
                tmpImage->flipY();
#else
            auto tmpFBO = new RenderTarget(Driver);
            auto prevFBO = ctxt->getRenderTarget();
            ctxt->setRenderTarget(tmpFBO);

            tmpFBO->setColorTextures({this}, {}, mipLevel);

            IImage *tmpImage = Driver->createImage(ECF_A8R8G8B8, lockImageSize);

            glReadPixels(0, 0, lockImageSize.Width, lockImageSize.Height,
                GL_RGBA, GL_UNSIGNED_BYTE, tmpImage->getData());

            tmpFBO->setColorTextures({}, {});

            ctxt->setRenderTarget(prevFBO);

            tmpFBO->drop();

            if (IsRenderTarget)
                tmpImage->flipY();

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
                LockImage->drop();
                LockImage = nullptr;
                break;
            }
            tmpImage->drop();
#endif
		}
	}

	return (LockImage) ? LockImage->getData() : nullptr;
}

void GLTexture::unlock()
{
	if (!LockImage)
		return;

	if (!LockReadOnly) {
        auto prevTexture = Driver->getContext()->getTextureUnit(0);
		Driver->getContext()->setTextureUnit(0, this);

        uploadTexture(LockLayer, LockMipLevel, LockImage->getData());

		Driver->getContext()->setTextureUnit(0, prevTexture);
	}

	LockImage->drop();

	LockReadOnly = false;
	LockImage = nullptr;
	LockLayer = 0;
}

void GLTexture::regenerateMipMaps()
{
    if (!TexSettings.HasMipMaps || (Size.Width <= 1 && Size.Height <= 1))
		return;

    auto prevTexture = Driver->getContext()->getTextureUnit(0);
	Driver->getContext()->setTextureUnit(0, this);

    glGenerateMipmap(toGLTexType[Type]);
	TEST_GL_ERROR(Driver);

	Driver->getContext()->setTextureUnit(0, prevTexture);
}

core::dimension2du GLTexture::getMipMapsSize(u32 mipLevel)
{
    u32 w = Size.Width >> mipLevel;
    u32 h = Size.Height >> mipLevel;
    return core::dimension2du(w > 0 ? w : 1, h > 0 ? h : 1);
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
        Size.Height = (u32)(features.MaxTextureSize / ratio);
	} else if (Size.Height > features.MaxTextureSize) {
		Size.Height = features.MaxTextureSize;
        Size.Width = (u32)(features.MaxTextureSize * ratio);
	}

	bool needSquare = (Type == ETT_CUBEMAP);
	Size = Size.getOptimalSize(false, needSquare, true, features.MaxTextureSize);

	Pitch = Size.Width * pixelFormatsInfo[ColorFormat].size / 8;
}

void GLTexture::genTexture()
{
    glGenTextures(1, &TexID);
    TEST_GL_ERROR(Driver);

    if (!TexID) {
        g_irrlogger->log("GLTexture: texture not created", ELL_ERROR);
        return;
    }
}

void GLTexture::initTexture()
{
    if (!TexSettings.IsRenderTarget) {
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if (TexSettings.HasMipMaps) {
            if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
                glHint(GL_GENERATE_MIPMAP_HINT, GL_FASTEST);
            else if (Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY))
                glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
            else
                glHint(GL_GENERATE_MIPMAP_HINT, GL_DONT_CARE);
        }
        TEST_GL_ERROR(Driver);
    }
    else if (Type != ETT_2D_MS) {
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(toGLTexType[Type], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        TexSettings.WrapU = ETC_CLAMP_TO_EDGE;
        TexSettings.WrapV = ETC_CLAMP_TO_EDGE;
        TexSettings.WrapW = ETC_CLAMP_TO_EDGE;
    }

    u8 levels = 1;
    if (TexSettings.HasMipMaps) {
        levels = core::max_(TexSettings.MaxMipLevel,
            (u8)core::u32_log2(core::max_(Size.Width, Size.Height))) + 1;

        glTexParameteri(toGLTexType[Type], GL_TEXTURE_MAX_LEVEL, (s32)(levels-1));
        TEST_GL_ERROR(Driver);
    }

    //if (InternalFormat == GL_BGRA && Driver->getVersion().Major >= 3)
    //	use_tex_storage = false;

    auto &formatInfo = GLSpecificInfo::TextureFormats[ColorFormat];

    u8 layers = Type == ETT_CUBEMAP ? 6 : 1;

	switch (Type) {
	case ETT_2D:
    case ETT_CUBEMAP:
        for (u8 i = 0; i < layers; i++) {
#ifdef _IRR_COMPILE_WITH_OPENGL3_
            glTexStorage2D(getTextureTarget(Type, i), levels, formatInfo.InternalFormat,
                Size.Width, Size.Height);
#else
            glTexImage2D(getTextureTarget(Type, i), 0, formatInfo.InternalFormat,
                Size.Width, Size.Height, 0, formatInfo.PixelFormat, formatInfo.PixelType, nullptr);
#endif
            TEST_GL_ERROR(Driver);
        }
		break;
	case ETT_2D_MS: {
		GLint max_samples = 0;
		glGetIntegerv(GL_MAX_SAMPLES, &max_samples);
		MSAA = core::min_(MSAA, (uint8_t)max_samples);

#ifdef _IRR_COMPILE_WITH_OPENGL3_
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, formatInfo.InternalFormat,
                                Size.Width, Size.Height, GL_TRUE);
#else
        glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA, formatInfo.InternalFormat,
                                Size.Width, Size.Height, GL_TRUE);
#endif
		TEST_GL_ERROR(Driver);
		break;
	}
	default:
		assert(false);
		break;
	}

    if (!NamedPath.getInternalName().empty())
        Driver->GLInfo->ObjectLabel(GL_TEXTURE, TexID, NamedPath.getInternalName().c_str());
}

void GLTexture::uploadTexture(u32 layer, u32 level, void *data)
{
	if (!data)
		return;

    core::dimension2du imageSize(getMipMapsSize(level));

	CImage *tmpImage = nullptr;
	void *tmpData = data;

    auto &formatInfo = GLSpecificInfo::TextureFormats[ColorFormat];

    if (formatInfo.Converter) {
        tmpImage = new CImage(ColorFormat, imageSize);
		tmpData = tmpImage->getData();
        formatInfo.Converter(data, imageSize.getArea(), tmpData);
	}

    glTexSubImage2D(getTextureTarget(Type, layer), level, 0, 0, imageSize.Width, imageSize.Height,
                    formatInfo.PixelFormat, formatInfo.PixelType, tmpData);

	TEST_GL_ERROR(Driver);

    if (formatInfo.Converter)
        delete tmpImage;
}

} // end namespace video
