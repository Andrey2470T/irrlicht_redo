// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2014 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include "Device/SDLDeviceParameters.h"
#include "../src/Video/VBO.h"
#include "../src/Video/MaterialSystem.h"
#include "../src/Video/Drawer.h"
#include "IO/IFileSystem.h"
#include "Utils/irrArray.h"
#include "Utils/irrString.h"
#include "Video/OpenGLFeatures.h"
#include "Mesh/IMesh.h"
#include "Mesh/IMeshBuffer.h"
#include "Mesh/S3DVertex.h"
#include "Mesh/SVertexIndex.h"
#include "Utils/fast_atof.h"
#include "Enums/EVideoTypes.h"
#include "Utils/matrix4.h"
#include "Mesh/MeshManipulator.h"
#include "DrawContext.h"
#include "Video/RenderTarget.h"
#include "Video/Texture.h"
#include "Image/Image.h"
#include <memory>

namespace io
{
class IWriteFile;
class IReadFile;
}
class SDLDevice;
namespace video
{
struct VertexType;
class GLSpecificInfo;

const c8 *const FogTypeNames[] = {
		"FogExp",
		"FogLinear",
		"FogExp2",
		0,
	};

struct SFrameStats {
	//! Number of draw calls
	u32 Drawcalls = 0;
	//! Count of primitives drawn
	u32 PrimitivesDrawn = 0;
	//! Number of hardware buffers uploaded (new or updated)
	u32 HWBuffersUploaded = 0;
	//! Number of active hardware buffers
	u32 HWBuffersActive = 0;
};

class VideoDriver : public IReferenceCounted, public MaterialSystem, public Drawer
{
	friend class GLTexture;

protected:
    //! constructor
	VideoDriver(const SDLDeviceParameters &params, io::IFileSystem *io, SDLDevice *device);

public:
	//! destructor
    ~VideoDriver();

	static VideoDriver *create(const SDLDeviceParameters &params, io::IFileSystem *io, SDLDevice *device);

	bool beginScene(u16 clearFlag, SColor clearColor = SColor(255, 0, 0, 0), f32 clearDepth = 1.f, u8 clearStencil = 0,
			core::rect<s32> *sourceRect = 0);

	//! Alternative beginScene implementation. Can't clear stencil buffer, but otherwise identical to other beginScene
	bool beginScene(SColor clearColor = SColor(255, 0, 0, 0))
	{
		return beginScene(ECBF_COLOR | ECBF_DEPTH, clearColor);
	}

	bool endScene();

	E_DRIVER_TYPE getDriverType() const
	{
		return DriverType;
	}

	//! sets transformation
	void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4 &mat);

	/// Links a hardware buffer to either a vertex or index buffer
	struct SHWBufferLink
	{
		SHWBufferLink(const scene::IVertexBuffer *vb) :
				VertexBuffer(vb), IsVertex(true)
		{
			if (VertexBuffer) {
				VertexBuffer->grab();
				VertexBuffer->setHWBuffer(this);
			}
		}
		SHWBufferLink(const scene::IIndexBuffer *ib) :
				IndexBuffer(ib), IsVertex(false)
		{
			if (IndexBuffer) {
				IndexBuffer->grab();
				IndexBuffer->setHWBuffer(this);
			}
		}

        ~SHWBufferLink()
		{
			if (IsVertex && VertexBuffer) {
				VertexBuffer->setHWBuffer(nullptr);
				VertexBuffer->drop();
			} else if (!IsVertex && IndexBuffer) {
				IndexBuffer->setHWBuffer(nullptr);
				IndexBuffer->drop();
			}
		}

		union {
			const scene::IVertexBuffer *VertexBuffer;
			const scene::IIndexBuffer *IndexBuffer;
		};
		size_t ListPosition = static_cast<size_t>(-1);
		u32 ChangedID = 0;
		bool IsVertex;

		OpenGLVBO Vbo;
	};

	bool updateVertexHardwareBuffer(SHWBufferLink *HWBuffer);
	bool updateIndexHardwareBuffer(SHWBufferLink *HWBuffer);

	//! updates hardware buffer if needed
	bool updateHardwareBuffer(SHWBufferLink *HWBuffer);

	void updateHardwareBuffer(const scene::IVertexBuffer *vb);

	void updateHardwareBuffer(const scene::IIndexBuffer *ib);

	//! Create hardware buffer from vertex buffer
	SHWBufferLink *createHardwareBuffer(const scene::IVertexBuffer *vb);

	//! Create hardware buffer from index buffer
	SHWBufferLink *createHardwareBuffer(const scene::IIndexBuffer *ib);

	//! Delete hardware buffer (only some drivers can)
	void deleteHardwareBuffer(SHWBufferLink *HWBuffer);

	void deleteAllTextures();
	void removeAllRenderTargets();
	void removeRenderTarget(RenderTarget *renderTarget);
	void removeAllHardwareBuffers();
	bool isHardwareBufferRecommend(const scene::IVertexBuffer *mb);
	bool isHardwareBufferRecommend(const scene::IIndexBuffer *mb);

	u32 getTextureCount() const
	{
		return Textures.size();
	}
    GLTexture *addTexture(const core::dimension2d<u32> &size, const io::path &name, ECOLOR_FORMAT format = ECF_A8R8G8B8);
    GLTexture *addTexture(const io::path &name, Image *image);

    GLTexture *addTextureCubemap(const io::path &name, Image *imagePosX, Image *imageNegX, Image *imagePosY,
            Image *imageNegY, Image *imagePosZ, Image *imageNegZ);

    GLTexture *addTextureCubemap(const u32 sideLen, const io::path &name, ECOLOR_FORMAT format = ECF_A8R8G8B8);

    void addTexture(GLTexture *surface);

	void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);
	bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag) const;

	bool queryTextureFormat(ECOLOR_FORMAT format) const;

	void setFog(SColor color = SColor(0, 255, 255, 255),
			E_FOG_TYPE fogType = EFT_FOG_LINEAR,
			f32 start = 50.0f, f32 end = 100.0f, f32 density = 0.01f,
			bool pixelFog = false, bool rangeFog = false);

	void getFog(SColor &color, E_FOG_TYPE &fogType,
			f32 &start, f32 &end, f32 &density,
			bool &pixelFog, bool &rangeFog);

	scene::MeshManipulator *getMeshManipulator() const
    {
        return MeshManipulator;
    }
    io::IFileSystem *getFileSystem() const
    {
        return FileSystem;
    }

	RenderTarget *addRenderTarget();

	const std::string &getName();
	const std::string &getVendorInfo();
	const OpenGLVersion &getVersion() const;
	const OpenGLFeatures &getFeatures() const;

	void setMinHardwareBufferVertexCount(u32 count);

	//! returns screen size
	const core::dimension2d<u32> &getScreenSize() const
	{
		return ScreenSize;
	}

	//! get current render target
	RenderTarget *getCurrentRenderTarget() const
	{
		return CurrentRenderTarget;
	}

	SFrameStats getFrameStats() const
	{
		return FrameStats;
	}

	const core::dimension2d<u32> &getCurrentRenderTargetSize() const;

	//! Returns the maximum texture size supported.
	core::dimension2du getMaxTextureSize() const;

	const core::rect<s32> &getViewPort() const
	{
		return ViewPort;
	}
	//! sets a viewport
	void setViewPort(const core::rect<s32> &area);

	//! Only used internally by the engine
	void OnResize(const core::dimension2d<u32> &size);

	//! Returns the transformation set by setTransform
	const core::matrix4 &getTransform(E_TRANSFORMATION_STATE state) const;

    GLTexture *addRenderTargetTexture(const core::dimension2d<u32> &size,
			const io::path &name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

    GLTexture *addRenderTargetTextureMs(const core::dimension2d<u32> &size, u8 msaa,
			const io::path &name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! Creates a render target texture for a cubemap
    GLTexture *addRenderTargetTextureCubemap(const u32 sideLen,
			const io::path &name, const ECOLOR_FORMAT format);

	//! looks if the image is already loaded
    GLTexture *findTexture(const io::path &filename);

	//! loads a Texture
    GLTexture *getTexture(const io::path &filename);

	bool setRenderTargetEx(RenderTarget *target, u16 clearFlag, SColor clearColor = SColor(255, 0, 0, 0),
			f32 clearDepth = 1.f, u8 clearStencil = 0);

    bool setRenderTarget(GLTexture *texture, u16 clearFlag = ECBF_COLOR | ECBF_DEPTH, SColor clearColor = SColor(255, 0, 0, 0),
		f32 clearDepth = 1.f, u8 clearStencil = 0);

	//! Sets a new render target.
	//! Prefer to use the setRenderTarget function taking flags as parameter as this one can't clear the stencil buffer.
	//! It's still offered for backward compatibility.
	bool setRenderTarget(GLTexture *texture, bool clearBackBuffer, bool clearZBuffer, SColor color = SColor(255, 0, 0, 0));

	//! Returns an image created from the last rendered frame.
    Image *createScreenShot(video::ECOLOR_FORMAT format = video::ECF_UNKNOWN, video::E_RENDER_TARGET target = video::ERT_FRAME_BUFFER);

	//! checks if an OpenGL error has happened and prints it, use via testGLError().
	// Does *nothing* unless in debug mode.
	bool testGLError(const char *file, int line);

    void removeTexture(GLTexture *texture);

	//! Used by some SceneNodes to check if a material should be rendered in the transparent render pass
	bool needsTransparentRenderPass(const video::SMaterial &material) const;

	DrawContext *getContext() const;

	//! Only used by the engine internally.
	void setAllowZWriteOnTransparent(bool flag)
	{
		AllowZWriteOnTransparent = flag;
	}

private:
    bool genericDriverInit(const core::dimension2d<u32> &screenSize, bool stencilBuffer);

	bool uploadHardwareBuffer(OpenGLVBO &vbo, const void *buffer, size_t bufferSize, scene::E_HARDWARE_MAPPING hint);

	SHWBufferLink *getBufferLink(const scene::IVertexBuffer *mb);
	SHWBufferLink *getBufferLink(const scene::IIndexBuffer *mb);
	void registerHardwareBuffer(SHWBufferLink *HWBuffer);
	void expireHardwareBuffers();

	inline void accountHWBufferUpload(u32 size)
	{
		FrameStats.HWBuffersUploaded++;
		(void)size;
	}

	//! Same as `CacheHandler->setViewport`, but also sets `ViewPort`
    void setViewPortRaw(u32 width, u32 height);

	E_DRIVER_TYPE DriverType;
	std::unique_ptr<GLSpecificInfo> GLInfo;
	std::unique_ptr<DrawContext> Context;
	SDLDeviceParameters Params;

	u8 AntiAlias;

	core::matrix4 TextureFlipMatrix;

	struct SSurface
	{
		io::path Name;
        GLTexture *Surface;

		bool operator<(const SSurface &other) const
		{
			return Name < other.Name;
		}
	};

	core::array<SSurface> Textures;

	core::array<RenderTarget *> RenderTargets;
	RenderTarget *SharedRenderTarget;
    core::array<GLTexture *> SharedDepthTextures;
	RenderTarget *CurrentRenderTarget;
	core::dimension2d<u32> CurrentRenderTargetSize;

	std::vector<SHWBufferLink *> HWBufferList;

	io::IFileSystem *FileSystem;
	scene::MeshManipulator *MeshManipulator;

	core::rect<s32> ViewPort;
	core::dimension2d<u32> ScreenSize;

	SFrameStats FrameStats;

	u32 MinVertexCountForVBO;
	u32 TextureCreationFlags;

	f32 FogStart;
	f32 FogEnd;
	f32 FogDensity;
	SColor FogColor;

	E_FOG_TYPE FogType;
	bool PixelFog;
	bool RangeFog;

	friend class MaterialRenderer;
	friend class MaterialSystem;
	friend class Drawer;

	core::matrix4 Matrices[ETS_COUNT];

	bool Transformation3DChanged;
	io::path OGLES2ShaderPath;

	SDLDevice *Device;

	bool EnableErrorTest;
};

} // end namespace video
