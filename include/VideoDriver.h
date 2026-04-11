// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2014 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include "SDLDeviceParameters.h"
#include "../src/Video/VBO.h"
#include "../src/Video/MaterialSystem.h"
#include "IFileSystem.h"
#include "irrArray.h"
#include "irrString.h"
#include "OpenGLFeatures.h"
#include "IMesh.h"
#include "IMeshBuffer.h"
#include "S3DVertex.h"
#include "SVertexIndex.h"
#include "fast_atof.h"
#include "EVideoTypes.h"
#include "matrix4.h"
#include "IMeshManipulator.h"
#include "DrawContext.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Image.h"
#include <memory>

namespace io
{
class IWriteFile;
class IReadFile;
}
class SDLDevice;
namespace video
{
class IImageLoader;
class IImageWriter;
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

class VideoDriver : public virtual IReferenceCounted, public MaterialSystem
{
	friend class GLTexture;

protected:
    //! constructor
	VideoDriver(const SDLDeviceParameters &params, io::IFileSystem *io, SDLDevice *device);

public:
	//! destructor
	virtual ~VideoDriver();

	static VideoDriver *create(const SDLDeviceParameters &params, io::IFileSystem *io, SDLDevice *device);

	bool beginScene(u16 clearFlag, SColor clearColor = SColor(255, 0, 0, 0), f32 clearDepth = 1.f, u8 clearStencil = 0,
			core::rect<s32> *sourceRect = 0);

	//! Alternative beginScene implementation. Can't clear stencil buffer, but otherwise identical to other beginScene
	bool beginScene(bool backBuffer, bool zBuffer, SColor color = SColor(255, 0, 0, 0),
			core::rect<s32> *sourceRect = 0)
	{
		u16 flag = 0;

		if (backBuffer)
			flag |= ECBF_COLOR;

		if (zBuffer)
			flag |= ECBF_DEPTH;

		return beginScene(flag, color, 1.f, 0, sourceRect);
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

		virtual ~SHWBufferLink()
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

    bool checkImage(const std::vector<Image*> &image) const;

    GLTexture *addTexture(const io::path &name, Image *image);

    virtual GLTexture *addTextureCubemap(const io::path &name, Image *imagePosX, Image *imageNegX, Image *imagePosY,
            Image *imageNegY, Image *imagePosZ, Image *imageNegZ);

    GLTexture *addTextureCubemap(const u32 sideLen, const io::path &name, ECOLOR_FORMAT format = ECF_A8R8G8B8);

    void addTexture(GLTexture *surface);

	void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);
	bool getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag) const;

	bool queryTextureFormat(ECOLOR_FORMAT format) const;

    Image *createImageFromFile(const io::path &filename);

    Image *createImageFromFile(io::IReadFile *file);

	//! Writes the provided image to disk file
    bool writeImageToFile(Image *image, const io::path &filename, u32 param = 0);

	//! Writes the provided image to a file.
    bool writeImageToFile(Image *image, io::IWriteFile *file, u32 param = 0);

	//! Creates a software image from a byte array.
	/** \param useForeignMemory: If true, the image will use the data pointer
	directly and own it from now on, which means it will also try to delete [] the
	data when the image will be destructed. If false, the memory will by copied. */
    Image *createImageFromData(ECOLOR_FORMAT format,
		const core::dimension2d<u32> &size, void *data, bool ownForeignMemory = false,
		bool deleteMemory = true);

	//! Creates an empty software image.
    Image *createImage(ECOLOR_FORMAT format, const core::dimension2d<u32> &size);

	//! Creates a software image from part of a texture.
    Image *createImage(GLTexture *texture,
		const core::position2d<s32> &pos,
		const core::dimension2d<u32> &size);

	void drawMeshBuffer(const scene::IMeshBuffer *mb)
	{
		if (!mb)
			return;
		drawBuffers(mb->getVertexBuffer(), mb->getIndexBuffer(),
			mb->getPrimitiveCount(), mb->getPrimitiveType());
	}

	void drawMeshBufferNormals(const scene::IMeshBuffer *mb, f32 length = 10.f,
		SColor color = 0xffffffff);

	void setFog(SColor color = SColor(0, 255, 255, 255),
			E_FOG_TYPE fogType = EFT_FOG_LINEAR,
			f32 start = 50.0f, f32 end = 100.0f, f32 density = 0.01f,
			bool pixelFog = false, bool rangeFog = false);

	void getFog(SColor &color, E_FOG_TYPE &fogType,
			f32 &start, f32 &end, f32 &density,
			bool &pixelFog, bool &rangeFog);

	scene::IMeshManipulator *getMeshManipulator();

	bool checkPrimitiveCount(u32 prmcnt) const;

	void drawBuffers(const scene::IVertexBuffer *vb,
		const scene::IIndexBuffer *ib, u32 primCount,
		scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES);

	RenderTarget *addRenderTarget();

	//! draws a vertex primitive list
	void drawVertexPrimitiveList(const void *vertices, u32 vertexCount,
			const void *indexList, u32 primitiveCount,
			E_VERTEX_TYPE vType = EVT_STANDARD, scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES,
			E_INDEX_TYPE iType = EIT_16BIT);

	//! draws a vertex primitive list in 2d
	void draw2DVertexPrimitiveList(const void *vertices, u32 vertexCount,
			const void *indexList, u32 primitiveCount,
			E_VERTEX_TYPE vType = EVT_STANDARD, scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES,
			E_INDEX_TYPE iType = EIT_16BIT);

	//! Draws an indexed triangle list.
	/** Note that there may be at maximum 65536 vertices, because
	the index list is an array of 16 bit values each with a maximum
	value of 65536. If there are more than 65536 vertices in the
	list, results of this operation are not defined.
	\param vertices Pointer to array of vertices.
	\param vertexCount Amount of vertices in the array.
	\param indexList Pointer to array of indices.
	\param triangleCount Amount of Triangles. Usually amount of indices / 3. */
	void drawIndexedTriangleList(const S3DVertex *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, EVT_STANDARD, scene::EPT_TRIANGLES, EIT_16BIT);
	}

	//! Draws an indexed triangle list.
	/** Note that there may be at maximum 65536 vertices, because
	the index list is an array of 16 bit values each with a maximum
	value of 65536. If there are more than 65536 vertices in the
	list, results of this operation are not defined.
	\param vertices Pointer to array of vertices.
	\param vertexCount Amount of vertices in the array.
	\param indexList Pointer to array of indices.
	\param triangleCount Amount of Triangles. Usually amount of indices / 3. */
	void drawIndexedTriangleList(const S3DVertex2TCoords *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, EVT_2TCOORDS, scene::EPT_TRIANGLES, EIT_16BIT);
	}

	//! Draws an indexed triangle list.
	/** Note that there may be at maximum 65536 vertices, because
	the index list is an array of 16 bit values each with a maximum
	value of 65536. If there are more than 65536 vertices in the
	list, results of this operation are not defined.
	\param vertices Pointer to array of vertices.
	\param vertexCount Amount of vertices in the array.
	\param indexList Pointer to array of indices.
	\param triangleCount Amount of Triangles. Usually amount of indices / 3. */
	void drawIndexedTriangleList(const S3DVertexTangents *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, EVT_TANGENTS, scene::EPT_TRIANGLES, EIT_16BIT);
	}

	//! Draws an indexed triangle fan.
	/** Note that there may be at maximum 65536 vertices, because
	the index list is an array of 16 bit values each with a maximum
	value of 65536. If there are more than 65536 vertices in the
	list, results of this operation are not defined.
	\param vertices Pointer to array of vertices.
	\param vertexCount Amount of vertices in the array.
	\param indexList Pointer to array of indices.
	\param triangleCount Amount of Triangles. Usually amount of indices - 2. */
	void drawIndexedTriangleFan(const S3DVertex *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, EVT_STANDARD, scene::EPT_TRIANGLE_FAN, EIT_16BIT);
	}

	//! Draws an indexed triangle fan.
	/** Note that there may be at maximum 65536 vertices, because
	the index list is an array of 16 bit values each with a maximum
	value of 65536. If there are more than 65536 vertices in the
	list, results of this operation are not defined.
	\param vertices Pointer to array of vertices.
	\param vertexCount Amount of vertices in the array.
	\param indexList Pointer to array of indices.
	\param triangleCount Amount of Triangles. Usually amount of indices - 2. */
	void drawIndexedTriangleFan(const S3DVertex2TCoords *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, EVT_2TCOORDS, scene::EPT_TRIANGLE_FAN, EIT_16BIT);
	}

	//! Draws an indexed triangle fan.
	/** Note that there may be at maximum 65536 vertices, because
	the index list is an array of 16 bit values each with a maximum
	value of 65536. If there are more than 65536 vertices in the
	list, results of this operation are not defined.
	\param vertices Pointer to array of vertices.
	\param vertexCount Amount of vertices in the array.
	\param indexList Pointer to array of indices.
	\param triangleCount Amount of Triangles. Usually amount of indices - 2. */
	void drawIndexedTriangleFan(const S3DVertexTangents *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, EVT_TANGENTS, scene::EPT_TRIANGLE_FAN, EIT_16BIT);
	}

	//! draws an 2d image
    void draw2DImage(const GLTexture *texture, const core::position2d<s32> &destPos, bool useAlphaChannelOfTexture = false);

    void draw2DImage(const GLTexture *texture,
			const core::position2d<s32> &destPos,
			const core::rect<s32> &sourceRect, const core::rect<s32> *clipRect = 0,
			SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false);

    void draw2DImage(const GLTexture *texture, const core::rect<s32> &destRect,
			const core::rect<s32> &sourceRect, const core::rect<s32> *clipRect = 0,
			const video::SColor *const colors = 0, bool useAlphaChannelOfTexture = false);

	// internally used
    void draw2DImage(const GLTexture *texture, u32 layer, bool flip);

    void draw2DImageBatch(const GLTexture *texture,
			const core::array<core::position2d<s32>> &positions,
			const core::array<core::rect<s32>> &sourceRects,
			const core::rect<s32> *clipRect,
			SColor color,
			bool useAlphaChannelOfTexture);

	//! draw an 2d rectangle
	void draw2DRectangle(SColor color, const core::rect<s32> &pos,
			const core::rect<s32> *clip = 0);

	//! Draws an 2d rectangle with a gradient.
	void draw2DRectangle(const core::rect<s32> &pos,
			SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
			const core::rect<s32> *clip = 0);

	//! Draws a 2d line.
	void draw2DLine(const core::position2d<s32> &start,
			const core::position2d<s32> &end,
			SColor color = SColor(255, 255, 255, 255));

	//! Draws a 3d line.
	void draw3DLine(const core::vector3df &start,
			const core::vector3df &end,
			SColor color = SColor(255, 255, 255, 255));

	//! Draws a 3d axis aligned box.
	void draw3DBox(const core::aabbox3d<f32> &box,
			SColor color = SColor(255, 255, 255, 255));

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

	//! get color format of the current color buffer
	ECOLOR_FORMAT getColorFormat() const;

	//! Returns the transformation set by setTransform
	const core::matrix4 &getTransform(E_TRANSFORMATION_STATE state) const;

	//! Returns the maximum amount of primitives
	u32 getMaximalPrimitiveCount() const;

    virtual GLTexture *addRenderTargetTexture(const core::dimension2d<u32> &size,
			const io::path &name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

    virtual GLTexture *addRenderTargetTextureMs(const core::dimension2d<u32> &size, u8 msaa,
			const io::path &name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

	//! Creates a render target texture for a cubemap
    GLTexture *addRenderTargetTextureCubemap(const u32 sideLen,
			const io::path &name, const ECOLOR_FORMAT format);

	//! looks if the image is already loaded
    GLTexture *findTexture(const io::path &filename);

	//! loads a Texture
    GLTexture *getTexture(const io::path &filename);

	//! loads a Texture
    GLTexture *getTexture(io::IReadFile *file);

	bool setRenderTargetEx(RenderTarget *target, u16 clearFlag, SColor clearColor = SColor(255, 0, 0, 0),
			f32 clearDepth = 1.f, u8 clearStencil = 0);

    bool setRenderTarget(GLTexture *texture, u16 clearFlag = ECBF_COLOR | ECBF_DEPTH, SColor clearColor = SColor(255, 0, 0, 0),
		f32 clearDepth = 1.f, u8 clearStencil = 0);

	//! Sets a new render target.
	//! Prefer to use the setRenderTarget function taking flags as parameter as this one can't clear the stencil buffer.
	//! It's still offered for backward compatibility.
    bool setRenderTarget(GLTexture *texture, bool clearBackBuffer, bool clearZBuffer, SColor color = SColor(255, 0, 0, 0))
	{
		u16 flag = 0;

		if (clearBackBuffer)
			flag |= ECBF_COLOR;

		if (clearZBuffer)
			flag |= ECBF_DEPTH;

		return setRenderTarget(texture, flag, color);
	}

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
	virtual bool genericDriverInit(const core::dimension2d<u32> &screenSize, bool stencilBuffer);

	GLTexture *createDeviceDependentTexture(const io::path &name, E_TEXTURE_TYPE type,
        const std::vector<Image*> &images);

	//! opens the file and loads it into the surface
    GLTexture *loadTextureFromFile(io::IReadFile *file, const io::path &hashName = "");

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
	virtual void setViewPortRaw(u32 width, u32 height);

	void drawQuad(const VertexType &vertexType, const S3DVertex (&vertices)[4]);
	void drawArrays(scene::E_PRIMITIVE_TYPE primitiveType, const VertexType &vertexType, const void *vertices, int vertexCount);
	void drawElements(scene::E_PRIMITIVE_TYPE primitiveType, const VertexType &vertexType, const void *vertices, int vertexCount, const u16 *indices, int indexCount);
	void drawElements(scene::E_PRIMITIVE_TYPE primitiveType, const VertexType &vertexType, uintptr_t vertices, uintptr_t indices, int indexCount);

	void drawGeneric(const void *vertices, const void *indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType);

	void beginDraw(const VertexType &vertexType, uintptr_t verticesBase);
	void endDraw(const VertexType &vertexType);

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

    core::array<IImageLoader *> SurfaceLoader;
    core::array<IImageWriter *> SurfaceWriter;

	std::vector<SHWBufferLink *> HWBufferList;

	io::IFileSystem *FileSystem;
	scene::IMeshManipulator *MeshManipulator;

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

	friend class GLTexture;
	friend class MaterialRenderer;
	friend class MaterialSystem;

	core::matrix4 Matrices[ETS_COUNT];

	bool Transformation3DChanged;
	io::path OGLES2ShaderPath;

	//! Color buffer format
	ECOLOR_FORMAT ColorFormat;

	SDLDevice *Device;

	bool EnableErrorTest;

	OpenGLVBO QuadIndexVBO;
	void initQuadsIndices(u32 max_vertex_count = 65536);
};

} // end namespace video
