#include "Texture2D.h"

namespace render
{

Texture2D::Texture2D(const std::string &name, u32 width, u32 height, img::PixelFormat format)
	: Texture(name, width, height, format)
{
	initTexture();
}

Texture2D::Texture2D(const std::string &name, std::unique_ptr<img::Image> image, const TextureSettings &settings)
	: Texture(name, image->getWidth(), image->getHeight(), image->getFormat(), settings),
	  imgCache(image.release())
{
	initTexture(imgCache->getData());
}

void Texture2D::initTexture(void *data)
{
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);

	img::PixelFormatInfo &formatInfo = img::pixelFormatInfo.at(format);

    if (texSettings.isRenderTarget) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, formatInfo.internalFormat, width, height, 0, formatInfo.pixelFormat, formatInfo.pixelType, 0);
	}
	else {
		if (data == nullptr) {
			ErrorStream << "Texture2D::initTexture() the data is invalid\n";
			glBindTexture(GL_TEXTURE_2D, 0);
			return;
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGLMinFilter.at(texSettings.minF));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGLMagFilter.at(texSettings.magF));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGLWrap.at(texSettings.wrapU));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGLWrap.at(texSettings.wrapV));

		glTexImage2D(GL_TEXTURE_2D, 0, formatInfo.internalFormat, width, height, 0, formatInfo.pixelFormat, formatInfo.pixelType, data);

		if (texSettings.hasMipMaps) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (GLint)texSettings.maxMipLevel);
            glGenerateMipmap(GL_TEXTURE_2D);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0);

	TEST_GL_ERROR();
}

void Texture2D::uploadData(img::Image *img, img::ImageModifier *imgMod)
{
	uploadSubData(width, height, img, imgMod);
}

void Texture2D::uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod)
{
	if (img == nullptr) {
		ErrorStream << "Texture2D::uploadData() the image data is invalid\n";
		return;
	}

	if (texSettings.isRenderTarget) {
		ErrorStream << "Texture2D::uploadData() can not upload the user image data to the RTT\n";
		return;
	}

	if (img->getFormat() != format) {
		ErrorStream << "Texture2D::uploadData() the format of the uploaded image data is different\n";
		return;
	}

	utils::v2u imgSize = img->getSize();

    if (imgCache && imgMod) {
        utils::rectu srcRect(0, 0, imgSize.X, imgSize.Y);
        utils::rectu dstRect(x, y, imgSize.X, imgSize.Y);
        imgMod->copyTo(img, imgCache.get(), &srcRect, &dstRect);
    }

	img::PixelFormatInfo &formatInfo = img::pixelFormatInfo.at(format);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, (s32)x, (s32)y, (s32)imgSize.X, (s32)imgSize.Y,
		formatInfo.pixelFormat, formatInfo.pixelType, static_cast<void *>(img->getData()));

	glBindTexture(GL_TEXTURE_2D, 0);

	TEST_GL_ERROR();
}

img::Image *Texture2D::downloadData() const
{
	if (imgCache)
		return imgCache.get();
	else {
		img::Image *img = new img::Image(format, width, height);

		img::PixelFormatInfo &formatInfo = img::pixelFormatInfo.at(format);
		glBindTexture(GL_TEXTURE_2D, texID);
		glGetTexImage(GL_TEXTURE_2D, 0, formatInfo.pixelFormat, formatInfo.pixelType, img->getData());

		glBindTexture(GL_TEXTURE_2D, 0);

		return img;
	}

	TEST_GL_ERROR();
}

void Texture2D::regenerateMipMaps(u8 max_level)
{
	texSettings.maxMipLevel = max_level;

	glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, (s32)texSettings.maxMipLevel);
    glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	TEST_GL_ERROR();
}

}
