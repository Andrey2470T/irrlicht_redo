#include "Texture2D.h"
#include "Render/Common.h"

namespace render
{

Texture2D::Texture2D(const std::string &name, u32 width, u32 height, img::PixelFormat format, u8 msaa_n)
    : Texture(name, width, height, format), msaa(msaa_n)
{
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

    glBindTexture(tex2D(), texID);
    TEST_GL_ERROR();

    auto &formatInfo = img::pixelFormatInfo.at(format);

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
	}
	else {
        if (image == nullptr) {
            ErrorStream << "Texture2D::initTexture() the image cache is invalid\n";
            glBindTexture(GL_TEXTURE_2D, 0);
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

        u8 *data = image->getData();
        v2u size = image->getClipSize();

        if (format == img::PF_INDEX_RGBA8) {
            convertIndicesToColors(image->getPalette(), &data, size);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)formatInfo.internalFormat, size.X, size.Y, 0, formatInfo.pixelFormat, formatInfo.pixelType, data);

        if (format == img::PF_INDEX_RGBA8)
            delete[] data;

        TEST_GL_ERROR();

		if (texSettings.hasMipMaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (s32)texSettings.maxMipLevel);
            glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

    glBindTexture(tex2D(), 0);

	TEST_GL_ERROR();
}

void Texture2D::uploadData(img::Image *img, img::ImageModifier *imgMod)
{
	uploadSubData(width, height, img, imgMod);
}

void Texture2D::uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod)
{
	if (texSettings.isRenderTarget) {
		ErrorStream << "Texture2D::uploadData() can not upload the user image data to the RTT\n";
		return;
	}

	if (img->getFormat() != format) {
		ErrorStream << "Texture2D::uploadData() the format of the uploaded image data is different\n";
		return;
	}

    v2u pos = img->getClipPos();
    v2u size = img->getClipSize();

    if (imgCache && imgMod) {
        rectu srcRect(pos.X, pos.Y, size.X, size.Y);
        rectu dstRect(x, y, size.X, size.Y);
        imgMod->copyTo(img, imgCache.get(), &srcRect, &dstRect);
    }

    auto &formatInfo = img::pixelFormatInfo.at(format);

    auto data = img->getData();

    if (img->getFormat() == img::PF_INDEX_RGBA8) {
        convertIndicesToColors(img->getPalette(), &data, size);
    }
	glBindTexture(GL_TEXTURE_2D, texID);
    glTexSubImage2D(GL_TEXTURE_2D, 0, (s32)x, (s32)y, (s32)size.X, (s32)size.Y,
        formatInfo.pixelFormat, formatInfo.pixelType, static_cast<void *>(data));

    if (img->getFormat() == img::PF_INDEX_RGBA8)
         delete[] data;

	glBindTexture(GL_TEXTURE_2D, 0);

	TEST_GL_ERROR();
}

std::vector<img::Image *> Texture2D::downloadData()
{
    if (!imgCache) {
		img::Image *img = new img::Image(format, width, height);

        auto &formatInfo = img::pixelFormatInfo.at(format);
        glBindTexture(tex2D(), texID);
        glGetTexImage(tex2D(), 0, formatInfo.pixelFormat, formatInfo.pixelType, img->getData());

        glBindTexture(tex2D(), 0);

        imgCache.reset(img);

        TEST_GL_ERROR();
	}

    return {imgCache.get()};
}

void Texture2D::regenerateMipMaps()
{
    if (!texSettings.hasMipMaps) {
        ErrorStream << "Texture2D::regenerateMipMaps() mip maps are disabled\n";
        return;
    }

    glBindTexture(tex2D(), texID);
    glGenerateMipmap(tex2D());

    glBindTexture(tex2D(), 0);

	TEST_GL_ERROR();
}

void Texture2D::updateParameters(const TextureSettings &newTexSettings, bool updateLodBias, bool updateAnisotropy)
{
    if (texSettings.isRenderTarget) {
        ErrorStream << "Texture2D::updateParameters() can not update settings for RTT\n";
        return;
    }

    glBindTexture(GL_TEXTURE_2D, texID);
    if (texSettings.wrapU != newTexSettings.wrapU) {
        texSettings.wrapU = newTexSettings.wrapU;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
    }
    if (texSettings.wrapV != newTexSettings.wrapV) {
        texSettings.wrapV = newTexSettings.wrapV;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));
    }
    if (texSettings.minF != newTexSettings.minF) {
        texSettings.minF = newTexSettings.minF;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
    }
    if (texSettings.magF != newTexSettings.magF) {
        texSettings.magF = newTexSettings.magF;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
    }

    if (updateLodBias && texSettings.lodBias != newTexSettings.lodBias) {
        f32 clampedBias = std::clamp<f32>(newTexSettings.lodBias * 0.125, -texSettings.maxLodBias, texSettings.maxLodBias);
        texSettings.lodBias = clampedBias;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, clampedBias);
    }
    if (updateAnisotropy && texSettings.anisotropyFilter != newTexSettings.anisotropyFilter) {
        u8 clampedAnisotropy = std::clamp<u8>(newTexSettings.anisotropyFilter, 1, texSettings.maxAnisotropyFilter);
        texSettings.anisotropyFilter = clampedAnisotropy;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, clampedAnisotropy);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    TEST_GL_ERROR();
}

Texture2D *Texture2D::copy(const std::string &name)
{
	img::Image *cache = downloadData().at(0);
    std::string texName = name.empty() ? getName() : name;
    return new Texture2D(texName, std::unique_ptr<img::Image>(cache->copy()), texSettings);
}

void Texture2D::convertIndicesToColors(img::Palette *palette, u8 **data, v2u size)
{
    u8 *convdata = new u8[size.X * size.Y * 4];

    for (u32 i = 0; i < size.X * size.Y; i++) {
        auto found_color = palette->colors.at((*data)[i]);
        convdata[i*4] = found_color.R();
        convdata[i*4+1] = found_color.G();
        convdata[i*4+2] = found_color.B();
        convdata[i*4+3] = found_color.A();
    }

    *data = convdata;
}

}
