#pragma once

#include "Common.h"
#include "Texture.h"

namespace render
{

class FrameBuffer
{
	u32 fboID;

	u32 width;
	u32 height;

	u32 maxColorAttachments;

	std::vector<std::unique_ptr<Texture>> colorTextures;
	std::unique_ptr<Texture> depthStencilTexture;
public:
	FrameBuffer(u32 _width, u32 _height, u32 _maxColorAttachments);

	~FrameBuffer();

	void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	}

	void unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	utils::v2u getSize() const
	{
		return utils::v2u(width, height);
	}

	std::vector<const Texture*> getColorTextures() const
	{
		std::vector<const Texture*> readTextures(colorTextures.size());
		
		for (u8 i = 0; i < colorTextures.size(); i++)
			readTextures[i] = colorTextures[i];
		
		return readTextures;
	}

	void setColorTextures(const std::vector<Texture*> &textures);
};

}
