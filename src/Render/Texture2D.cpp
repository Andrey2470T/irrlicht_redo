#include "Texture2D.h"
#include "Image/Converting.h"
#include "Image/ImageLoader.h"
#include "Render/Common.h"

namespace render
{

Texture2D::Texture2D(const std::string &name, u32 width, u32 height, img::PixelFormat format, u8 msaa_n, u8 maxMipLevel)
    : Texture(name, width, height, format), msaa(msaa_n)
{
    if (maxMipLevel > 0) {
        texSettings.hasMipMaps = true;
        texSettings.maxMipLevel = maxMipLevel;
    }
    initTexture(nullptr);
}

Texture2D::Texture2D(const std::string &name, std::unique_ptr<img::Image> image, const TextureSettings &settings)
	: Texture(name, image->getWidth(), image->getHeight(), image->getFormat(), settings),
	  imgCache(std::move(image))
{
    initTexture(imgCache.get());
}

void Texture2D::initTexture(img::Image *image)
{
	glGenTextures(1, &texID);
    TEST_GL_ERROR();

    bind();

    auto formatInfo = img::pixelFormatInfo.at(format);

    if (texSettings.isRenderTarget) {
        if (msaa == 0) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexImage2D(GL_TEXTURE_2D, 0, formatInfo.internalFormat, width, height, 0, formatInfo.pixelFormat, formatInfo.pixelType, 0);
        }
        else
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, msaa, formatInfo.internalFormat, width, height, GL_TRUE);
        TEST_GL_ERROR();
	}
	else {
        if (image == nullptr) {
            ErrorStream << "Texture2D::initTexture() the image cache is invalid\n";
            glBindTexture(GL_TEXTURE_2D, 0);
            TEST_GL_ERROR();
            return;
        }
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));
        TEST_GL_ERROR();

        auto convImg = img::convertIndexImageToRGBA(image);
        formatInfo = convImg ? img::pixelFormatInfo.at(img::PF_RGBA8) : formatInfo;

        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)formatInfo.internalFormat, width, height, 0,
            formatInfo.pixelFormat, formatInfo.pixelType, convImg ? convImg->getData() : image->getData());
        TEST_GL_ERROR();

        if (convImg)
            delete convImg;

		if (texSettings.hasMipMaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (s32)texSettings.maxMipLevel);
            TEST_GL_ERROR();
            glGenerateMipmap(GL_TEXTURE_2D);
            TEST_GL_ERROR();
		}
	}

    unbind();
}

void Texture2D::bind()
{
    if (bound)
        return;
    glBindTexture(tex2D(), texID);
    TEST_GL_ERROR();
    bound = true;
}

void Texture2D::unbind()
{
    if (!bound)
        return;
    glBindTexture(tex2D(), 0);
    TEST_GL_ERROR();
    bound = false;
}

void Texture2D::uploadData(img::Image *img, img::ImageModifier *imgMod)
{
	uploadSubData(width, height, img, imgMod);
}

void Texture2D::uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod)
{
    v2u pos = img->getClipPos();
    v2u size = img->getClipSize();

    if (imgCache && imgMod) {
        rectu srcRect(pos.X, pos.Y, size.X, size.Y);
        rectu dstRect(x, y, size.X, size.Y);
        imgMod->copyTo(img, imgCache.get(), &srcRect, &dstRect);
    }

    auto formatInfo = img::pixelFormatInfo.at(format);

    auto convImg = img::convertIndexImageToRGBA(img);
    formatInfo = convImg ? img::pixelFormatInfo.at(img::PF_RGBA8) : formatInfo;

    bind();
    glTexSubImage2D(GL_TEXTURE_2D, 0, (s32)x, (s32)y, (s32)size.X, (s32)size.Y,
        formatInfo.pixelFormat, formatInfo.pixelType, convImg ? convImg->getData() : img->getData());
    TEST_GL_ERROR();

    if (convImg)
         delete convImg;

    unbind();
}

std::vector<img::Image *> Texture2D::downloadData()
{
    img::Image *cachedImg = nullptr;
    if (!imgCache) {
		img::Image *img = new img::Image(format, width, height);

        auto formatInfo = img::pixelFormatInfo.at(format);
        bind();

        if (format == img::PF_INDEX_RGBA8) {
            formatInfo = img::pixelFormatInfo.at(img::PF_RGBA8);
        }

        u8* tempData = new u8[width * height * formatInfo.size / 8];
        glGetTexImage(tex2D(), 0, formatInfo.pixelFormat, formatInfo.pixelType, tempData);
        TEST_GL_ERROR();

        if (format == img::PF_INDEX_RGBA8) {
            auto indexdata = convertRGBAImageDataToIndex(img->getPalette(), tempData, img->getSize(), img->getPitch());

            memcpy(img->getData(), indexdata, width * height);

            delete[] indexdata;
            delete[] tempData;
        }
        else {
            u32 dataSize = width * height * formatInfo.size / 8;
            memcpy(img->getData(), tempData, dataSize);
            delete[] tempData;
        }

        unbind();

        cachedImg = img;
        //imgCache.reset(img);
	}
    else
        cachedImg = imgCache.get();

    return {cachedImg};
}

void Texture2D::regenerateMipMaps()
{
    if (!texSettings.hasMipMaps) {
        ErrorStream << "Texture2D::regenerateMipMaps() mip maps are disabled\n";
        return;
    }

    bind();
    glGenerateMipmap(tex2D());
    TEST_GL_ERROR();

    unbind();
}

void Texture2D::updateParameters(const TextureSettings &newTexSettings, bool updateLodBias, bool updateAnisotropy)
{
    if (texSettings.isRenderTarget) {
        ErrorStream << "Texture2D::updateParameters() can not update settings for RTT\n";
        return;
    }

    bind();
    if (texSettings.wrapU != newTexSettings.wrapU) {
        texSettings.wrapU = newTexSettings.wrapU;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
        TEST_GL_ERROR();
    }
    if (texSettings.wrapV != newTexSettings.wrapV) {
        texSettings.wrapV = newTexSettings.wrapV;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));
        TEST_GL_ERROR();
    }
    if (texSettings.minF != newTexSettings.minF) {
        texSettings.minF = newTexSettings.minF;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
        TEST_GL_ERROR();
    }
    if (texSettings.magF != newTexSettings.magF) {
        texSettings.magF = newTexSettings.magF;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
        TEST_GL_ERROR();
    }

    if (updateLodBias && texSettings.lodBias != newTexSettings.lodBias) {
        f32 clampedBias = std::clamp<f32>(newTexSettings.lodBias * 0.125, -texSettings.maxLodBias, texSettings.maxLodBias);
        texSettings.lodBias = clampedBias;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, clampedBias);
        TEST_GL_ERROR();
    }
    if (updateAnisotropy && texSettings.anisotropyFilter != newTexSettings.anisotropyFilter) {
        u8 clampedAnisotropy = std::clamp<u8>(newTexSettings.anisotropyFilter, 1, texSettings.maxAnisotropyFilter);
        texSettings.anisotropyFilter = clampedAnisotropy;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, clampedAnisotropy);
        TEST_GL_ERROR();
    }

    unbind();
}

Texture2D *Texture2D::copy(const std::string &name)
{
	img::Image *cache = downloadData().at(0);
    std::string texName = name.empty() ? getName() : name;
    return new Texture2D(texName, std::unique_ptr<img::Image>(cache->copy()), texSettings);
}

}
