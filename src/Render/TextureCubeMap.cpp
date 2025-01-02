#include "TextureCubeMap.h"

namespace render
{

TextureCubeMap::TextureCubeMap(const std::string &name, u32 width, u32 height, img::PixelFormat format)
	: Texture(name, width, height, format)
{
	initTexture();
}

TextureCubeMap::TextureCubeMap(const std::string &name, std::array<std::unique_ptr<img::Image>, CMF_COUNT> images,
		const TextureSettings &settings)
    : Texture(name, images.at(0)->getWidth(), images.at(0)->getHeight(), images.at(0)->getFormat(), settings)
{
	std::array<img::Image *, CMF_COUNT> imgs;
	for (u8 i = 0; i < CMF_COUNT; i++) {
        imgCache[i] = std::unique_ptr<img::Image>(images.at(i).release());
		imgs[i] = imgCache[i].get();
	}

	initTexture(imgs);
}

void TextureCubeMap::initTexture(const std::array<img::Image *, CMF_COUNT> &data)
{
	glGenTextures(1, &texID);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	
    img::PixelFormatInfo &formatInfo = img::pixelFormatInfo.at(format);

    if (texSettings.isRenderTarget) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		for (u8 i = 0; i < CMF_COUNT; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                formatInfo.internalFormat, width, height, 0, formatInfo.pixelFormat, formatInfo.pixelType, 0);
	}
	else {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));

		for (u8 i = 0; i < CMF_COUNT; i++) {
			if (data[i] != nullptr)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, formatInfo.internalFormat,
					width, height, 0, formatInfo.pixelFormat, formatInfo.pixelType, static_cast<void*>(data[i]->getData()));
		}

		if (texSettings.hasMipMaps) {
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, (GLint)texSettings.maxMipLevel);
			glGenerateMipMap(GL_TEXTURE_CUBE_MAP);
		}
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	TEST_GL_ERROR();
}

}
