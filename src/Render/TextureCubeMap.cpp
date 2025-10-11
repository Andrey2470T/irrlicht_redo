#include "TextureCubeMap.h"
#include "Common.h"

namespace render
{

TextureCubeMap::TextureCubeMap(const std::string &name, u32 width, u32 height, img::PixelFormat format)
	: Texture(name, width, height, format)
{
	initTexture();
}

TextureCubeMap::TextureCubeMap(const std::string &name, std::array<img::Image *, CMF_COUNT> images,
		const TextureSettings &settings)
    : Texture(name, images.at(0)->getWidth(), images.at(0)->getHeight(), images.at(0)->getFormat(), settings)
{
	std::array<img::Image *, CMF_COUNT> imgs;
	for (u8 i = 0; i < CMF_COUNT; i++) {
        imgCache[i] = std::unique_ptr<img::Image>(images.at(i));
		imgs[i] = imgCache[i].get();
	}

	initTexture(imgs);
}

void TextureCubeMap::initTexture(const std::array<img::Image *, CMF_COUNT> &data)
{
	glGenTextures(1, &texID);
    TEST_GL_ERROR();
	
    bind();
	
    auto &formatInfo = img::pixelFormatInfo.at(format);

    if (texSettings.isRenderTarget) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        TEST_GL_ERROR();

        for (u8 i = 0; i < CMF_COUNT; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                formatInfo.internalFormat, width, height, 0, formatInfo.pixelFormat, formatInfo.pixelType, 0);
            TEST_GL_ERROR();
        }
	}
	else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
        TEST_GL_ERROR();
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));
        TEST_GL_ERROR();

		for (u8 i = 0; i < CMF_COUNT; i++) {
            if (data[i] != nullptr) {
                v2u size = data[i]->getClipSize();
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, formatInfo.internalFormat,
                    size.X, size.Y, 0, formatInfo.pixelFormat, formatInfo.pixelType, static_cast<void*>(data[i]->getData()));
                TEST_GL_ERROR();
            }
		}

		if (texSettings.hasMipMaps) {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, (GLint)texSettings.maxMipLevel);
            TEST_GL_ERROR();
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            TEST_GL_ERROR();
		}
	}

    unbind();
}

void TextureCubeMap::bind()
{
    if (bound)
        return;
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
    TEST_GL_ERROR();
    bound = true;
}

void TextureCubeMap::unbind()
{
    if (!bound)
        return;
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    TEST_GL_ERROR();
    bound = false;
}

std::vector<img::Image *> TextureCubeMap::downloadData()
{
    std::vector<img::Image *> imgs(CMF_COUNT);

    for (u8 i = 0; i < CMF_COUNT; i++) {
        if (!imgCache[i]) {
            img::Image *img = new img::Image(format, width, height);

            auto &formatInfo = img::pixelFormatInfo.at(format);

            bind();

            glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, formatInfo.pixelFormat, formatInfo.pixelType, img->getData());
            TEST_GL_ERROR();
            imgCache[i].reset(img);
        }
        imgs[i] = imgCache[i].get();
    }

    unbind();

    return imgs;
}

void TextureCubeMap::updateParameters(const TextureSettings &newTexSettings, bool updateLodBias, bool updateAnisotropy)
{
    if (texSettings.isRenderTarget) {
        ErrorStream << "TextureCubeMap::updateParameters() can not update settings for RTT\n";
        return;
    }

    bind();
    if (texSettings.wrapU != newTexSettings.wrapU) {
        texSettings.wrapU = newTexSettings.wrapU;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
        TEST_GL_ERROR();
    }
    if (texSettings.wrapV != newTexSettings.wrapV) {
        texSettings.wrapV = newTexSettings.wrapV;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));
        TEST_GL_ERROR();
    }
    if (texSettings.wrapW != newTexSettings.wrapW) {
        texSettings.wrapW = newTexSettings.wrapW;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, toGLWrap.at(texSettings.wrapW));
        TEST_GL_ERROR();
    }
    if (texSettings.minF != newTexSettings.minF) {
        texSettings.minF = newTexSettings.minF;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
        TEST_GL_ERROR();
    }
    if (texSettings.magF != newTexSettings.magF) {
        texSettings.magF = newTexSettings.magF;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
        TEST_GL_ERROR();
    }

    if (updateLodBias && texSettings.lodBias != newTexSettings.lodBias) {
        f32 clampedBias = std::clamp<f32>(newTexSettings.lodBias * 0.125, -texSettings.maxLodBias, texSettings.maxLodBias);
        texSettings.lodBias = clampedBias;
        glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_LOD_BIAS, clampedBias);
        TEST_GL_ERROR();
    }
    if (updateAnisotropy && texSettings.anisotropyFilter != newTexSettings.anisotropyFilter) {
        u8 clampedAnisotropy = std::clamp<u8>(newTexSettings.anisotropyFilter, 1, texSettings.maxAnisotropyFilter);
        texSettings.anisotropyFilter = clampedAnisotropy;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, clampedAnisotropy);
        TEST_GL_ERROR();
    }

    unbind();
}

}
