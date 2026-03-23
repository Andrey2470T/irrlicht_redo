#pragma once

#include "IReferenceCounted.h"
#include "EDriverTypes.h"
#include "irrArray.h"
#include "dimension2d.h"

namespace video
{

class ITexture;
class VideoDriver;

//! Enumeration of cube texture surfaces
enum E_CUBE_SURFACE
{
	ECS_POSX = 0,
	ECS_NEGX,
	ECS_POSY,
	ECS_NEGY,
	ECS_POSZ,
	ECS_NEGZ
};

//! Interface of a Render Target.
/** This is a framebuffer object (FBO) in OpenGL. */
class RenderTarget : public virtual IReferenceCounted
{
	u32 fboID;

	u32 width;
	u32 height;

	VideoDriver *driver;

	std::vector<ITexture*> colorTextures;
	std::vector<E_CUBE_SURFACE> colorCubeMapFaces;

	ITexture* depthStencilTexture = nullptr;
	E_CUBE_SURFACE depthStencilCubeMapFace;
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

	std::vector<ITexture*> getColorTextures() const
	{
		return colorTextures;
	}

	ITexture *getDepthStencilTexture() const
	{
		return depthStencilTexture;
	}

	void setColorTextures(
		const std::vector<ITexture*> &textures,
		const std::vector<E_CUBE_SURFACE> &cubeMapFaceMappings={},
		u8 mipLevel=0);
	void setDepthStencilTexture(
		ITexture *texture, E_CUBE_SURFACE dsCubeMapFace=ECS_POSX, u8 mipLevel=0);

	void setTextures(
		const std::vector<ITexture*> &_colorTextures, ITexture *_depthStencilTexture, u8 mipLevel=0)
	{
		setColorTextures(_colorTextures, {}, mipLevel);
		setDepthStencilTexture(_depthStencilTexture, ECS_POSX, mipLevel);
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
