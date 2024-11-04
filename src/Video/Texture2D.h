#pragma once

#include <memory>
#include "Texture.h"

namespace render
{

class Texture2D : public Texture
{
	GLuint id;

	TextureSettings texSettings;
	std::unique_ptr<Image> imgCache;
public:
	Texture2D(const std::string &name, u32 width, u32 height, TextureFormat format);
	Texture2D(const std::string &name, std::unique_ptr<Image> image, const TextureSettings &settings);

	~Texture()
	{
		glDeleteTextures(1, &id);
	}

	TextureType getType() const override
	{
		return TT_2D;
	}

	void bind() const override
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void unbind() const override
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void initTexture(void *data = nullptr);

	TEST_GL_ERROR();
};

}
