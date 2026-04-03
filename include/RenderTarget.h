#pragma once

#include "IReferenceCounted.h"
#include "EDriverTypes.h"
#include "irrArray.h"
#include "dimension2d.h"
#include "Texture.h"

namespace video
{

class VideoDriver;

//! Interface of a Render Target.
/** This is a framebuffer object (FBO) in OpenGL. */
class RenderTarget : public virtual IReferenceCounted
{
	u32 fboID;

	u32 width;
	u32 height;

	VideoDriver *driver;

	std::vector<GLTexture*> colorTextures;
	std::vector<E_CUBEMAP_FACE> colorCubeMapFaces;

	GLTexture* depthStencilTexture = nullptr;
	E_CUBEMAP_FACE depthStencilCubeMapFace;
public:
	RenderTarget(video::VideoDriver *_driver);

	~RenderTarget();

	u32 getID() const
	{
		return fboID;
	}

	void bind() const;

	void unbind() const;

	core::dimension2du getSize() const
	{
		return {width, height};
	}

	std::vector<GLTexture*> getColorTextures() const
	{
		return colorTextures;
	}

	GLTexture *getDepthStencilTexture() const
	{
		return depthStencilTexture;
	}

	void setColorTextures(
		const std::vector<GLTexture*> &textures,
		const std::vector<E_CUBEMAP_FACE> &cubeMapFaceMappings={},
		u8 mipLevel=0);
	void setDepthStencilTexture(
		GLTexture *texture, E_CUBEMAP_FACE dsCubeMapFace=ECMF_POS_X, u8 mipLevel=0);

	void setTextures(
		const std::vector<GLTexture*> &_colorTextures, GLTexture *_depthStencilTexture, u8 mipLevel=0)
	{
		setColorTextures(_colorTextures, {}, mipLevel);
		setDepthStencilTexture(_depthStencilTexture, ECMF_POS_X, mipLevel);
	}

	void blitTo(RenderTarget *target);

	bool operator==(const RenderTarget &other)
	{
		return fboID == other.fboID;
	}

	bool operator!=(const RenderTarget &other)
	{
		return fboID != other.fboID;
	}
private:
	bool checkStatus() const;
};

}
