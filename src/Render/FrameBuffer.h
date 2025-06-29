#pragma once

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
    FrameBuffer(u32 _maxColorAttachments);

	~FrameBuffer();

    u32 getID() const
    {
        return fboID;
    }

	void bind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	}

	void unbind() const
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    v2u getSize() const
	{
        return v2u(width, height);
	}

	std::vector<Texture*> getColorTextures() const
	{
		return colorTextures;
	}

	Texture *getDepthStencilTexture() const
	{
		return depthStencilTexture;
	}

    void setColorTextures(const std::vector<Texture*> &textures, const std::vector<CubeMapFace> &cubeMapFaceMappings={});
    void setDepthStencilTexture(Texture *texture, CubeMapFace dsCubeMapFace=CMF_COUNT);

    void blitTo(const FrameBuffer *target);
	
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
