#pragma once

#include "Texture.h"

namespace render
{

class Texture2D : public Texture
{
	GLuint id;

	TextureSettings texSettings;
	std::unique_ptr<img::Image> imgCache;
public:
	Texture2D(const std::string &name, u32 width, u32 height, img::PixelFormat format);
	Texture2D(const std::string &name, std::unique_ptr<img::Image> image, const TextureSettings &settings);

	~Texture()
	{
		glDeleteTextures(1, &id);
	}

	TextureType getType() const override
	{
		return TT_2D;
	}

	bool hasMipMaps() const override
	{
		return texSettings.hasMipMaps;
	}

	bool isRenderTarget() const override
	{
		return texSettings.isRenderTarget;
	}

	void bind() const override
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void unbind() const override
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void uploadData(img::Image *img, img::ImageModifier *imgMod = nullptr) override;
	void uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod = nullptr) override;

	img::Image *downloadData() const override;
	void regenerateMipMaps(u8 max_level) override;

	void initTexture(void *data = nullptr);
};

}
