#include "Texture2D.h"

namespace render
{

Texture2D::Texture2D(const std::string &name, u32 width, u32 height, TextureFormat format)
	: Texture(name, width, height, format)
{
	settings.hasMipMaps = false;
	settings.maxMipLevel = 0;
	isRenderTarget = true;
	
	initTexture();
}

Texture2D::Texture2D(const std::string &name, std::unique_ptr<Image> image, const TextureSettings &settings)
	: Texture(name, image->getWidth(), image->getHeight(), image->getFormat()),
	  imgCache(std::make_unique(image->getWidth(), image->getHeight(), image->getData())), texSettings(settings)
{
	initTexture(imgCache->getData());
}

void Texture2D::initTexture(void *data = nullptr)
{
	glGenTextures(1, &id);
	
	glBindTexture(GL_TEXTURE_2D, id);
	
	if (isRenderTarget) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		glTexImage2D(GL_TEXTURE_2D, 0, format_info.internalFormat, width, height, 0, format_info.pixelFormat, format_info.pixelType, 0);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));
	
		glTexImage2D(GL_TEXTURE_2D, 0, format_info.internalFormat, width, height, 0, format_info.pixelFormat, format_info.pixelType, data);
		
		if (texSettings.hasMipMaps) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)texSettings.maxMipLevel);
			glGenerateMipMaps(GL_TEXTURE_2D);
		}
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	TEST_GL_ERROR();
}

}
