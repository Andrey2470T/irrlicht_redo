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

	u8 maxColorAttachments;

	std::vector<Texture*> colorTextures;
	std::vector<CubeMapFace> colorCubeMapFaces;

	Texture* depthStencilTexture;
	CubeMapFace depthStencilCubeMapFace;
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

	std::vector<Texture*> getColorTextures() const
	{
		return colorTextures;
	}

	Texture *getDepthStencilTexture() const
	{
		return depthStencilTexture;
	}
	
	void clearBuffers(u16 flags, img::color8 color, f32 depth, u8 stencil);

	void setColorTextures(const std::vector<Texture*> &textures, const std::vector<CubeMapFace> &cubeMapFaceMappings);
	void setDepthStencilTexture(Texture *texture);
	
	bool operator==(const FrameBuffer *other)
	{
		return fboID == other->fboID;
	}
	
	bool operator!=(const FrameBuffer *other)
	{
		return fboID != other->fboID;
	}
private:
	bool checkStatus() const;
};

}
