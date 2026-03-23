// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2014 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "VideoDriver.h"
#include <cassert>
#include "IContextManager.h"

#include "RenderTarget.h"
#include "DrawContext.h"
#include "Video/COpenGLCoreTexture.h"

#include "MaterialRenderer.h"
#include "FixedPipelineRenderer.h"
#include "Renderer2D.h"
#include "VertexType.h"
#include "Common.h"

#include "EVertexAttributes.h"
#include "Image/CImage.h"
#include "Logger.h"
#include "Mesh/CMeshManipulator.h"
#include "Image/CImageLoaderJPG.h"
#include "Image/CImageLoaderPNG.h"
#include "Image/CImageLoaderTGA.h"
#include "Image/CImageWriterJPG.h"
#include "Image/CImageWriterPNG.h"


namespace video
{

VideoDriver::VideoDriver(const SIrrlichtCreationParameters &params, io::IFileSystem *io, IContextManager *contextManager) :
		MaterialSystem(this, io, params.OGLES2ShaderPath),
		Params(params), AntiAlias(params.AntiAlias), SharedRenderTarget(nullptr),
		CurrentRenderTarget(nullptr), CurrentRenderTargetSize(0, 0),
		FileSystem(io), MeshManipulator(nullptr),
		ViewPort(0, 0, 0, 0), ScreenSize(params.WindowSize),
		MinVertexCountForVBO(500), TextureCreationFlags(0),
		Transformation3DChanged(true), OGLES2ShaderPath(params.OGLES2ShaderPath),
		ColorFormat(ECF_R8G8B8), ContextManager(contextManager), EnableErrorTest(params.DriverDebug)
{
	setFog();

	setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);
	setTextureCreationFlag(ETCF_ALLOW_MEMORY_COPY, false);

	ViewPort = core::rect<s32>(core::position2d<s32>(0, 0), core::dimension2di(params.WindowSize));

	MeshManipulator = new scene::CMeshManipulator();

	if (FileSystem)
		FileSystem->grab();

	SurfaceLoader.push_back(new CImageLoaderTGA());
	SurfaceLoader.push_back(new CImageLoaderPng());
	SurfaceLoader.push_back(new CImageLoaderJPG());

	SurfaceWriter.push_back(new CImageWriterJPG());
	SurfaceWriter.push_back(new CImageWriterPNG());

	memset((void *)&ExposedData, 0, sizeof(ExposedData));

	if (!ContextManager)
		return;

	ContextManager->grab();
	ContextManager->generateSurface();
	ContextManager->generateContext();
	ExposedData = ContextManager->getContext();
	ContextManager->activateContext(ExposedData, false);

	testGLError();
}

VideoDriver::~VideoDriver()
{
	QuadIndexVBO.destroy();

	if (FileSystem)
		FileSystem->drop();

	if (MeshManipulator)
		MeshManipulator->drop();

	removeAllRenderTargets();
	deleteAllTextures();

	u32 i;
	for (i = 0; i < SurfaceLoader.size(); ++i)
		SurfaceLoader[i]->drop();

	for (i = 0; i < SurfaceWriter.size(); ++i)
		SurfaceWriter[i]->drop();

	removeAllHardwareBuffers();

	if (ContextManager) {
		ContextManager->destroyContext();
		ContextManager->destroySurface();
		ContextManager->terminate();
		ContextManager->drop();
	}
}

VideoDriver *VideoDriver::create(const SIrrlichtCreationParameters &params, io::IFileSystem *io, IContextManager *contextManager)
{
	g_irrlogger->log("Create VideoDriver", ELL_INFORMATION);
	auto driver = new VideoDriver(params, io, contextManager);
	DriverType = params.DriverType;
	driver->genericDriverInit(params.WindowSize, params.Stencilbuffer); // don't call in constructor, it uses virtual function calls of driver
	return driver;
}

void VideoDriver::initQuadsIndices(u32 max_vertex_count)
{
	u32 max_quad_count = max_vertex_count / 4;
	u32 indices_size = 6 * max_quad_count;
	if (indices_size == QuadIndexVBO.getSize() * sizeof(u16))
		return;
	// initialize buffer contents
	std::vector<u16> QuadsIndices;
	QuadsIndices.reserve(indices_size);
	for (u32 k = 0; k < max_quad_count; k++) {
		QuadsIndices.push_back(4 * k + 0);
		QuadsIndices.push_back(4 * k + 1);
		QuadsIndices.push_back(4 * k + 2);
		QuadsIndices.push_back(4 * k + 0);
		QuadsIndices.push_back(4 * k + 2);
		QuadsIndices.push_back(4 * k + 3);
	}
	QuadIndexVBO.upload(QuadsIndices.data(), QuadsIndices.size() * sizeof(u16),
		0, GL_STATIC_DRAW, true);
	assert(QuadIndexVBO.exists());
}

bool VideoDriver::genericDriverInit(const core::dimension2d<u32> &screenSize, bool stencilBuffer)
{
	GLInfo = std::make_unique<GLSpecificInfo>(stencilBuffer);

	initQuadsIndices();

	// reset cache handler
	Context.reset();
	Context = std::make_unique<DrawContext>(this);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	for (s32 i = 0; i < ETS_COUNT; ++i)
		setTransform(static_cast<E_TRANSFORMATION_STATE>(i), core::IdentityMatrix);

	glClearDepthf(1.0f);

	glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
	glFrontFace(GL_CW);

	// create material renderers
	createMaterialRenderers();

	// set the renderstates
	setRenderStates3DMode();

	// set fog mode
	setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

	// create matrix for flipping textures
	TextureFlipMatrix.buildTextureTransform(0.0f, core::vector2df(0, 0), core::vector2df(0, 1.0f), core::vector2df(1.0f, -1.0f));

	// We need to reset once more at the beginning of the first rendering.
	// This fixes problems with intermediate changes to the material during texture load.
	ResetRenderStates = true;

	testGLError();

	return true;
}

bool VideoDriver::beginScene(u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil, const SExposedVideoData &videoData, core::rect<s32> *sourceRect)
{
	FrameStats = {};

	if (ContextManager)
		ContextManager->activateContext(videoData, true);

	Context->clearBuffers(clearFlag, clearColor, clearDepth, clearStencil);

	return true;
}

bool VideoDriver::endScene()
{
	expireHardwareBuffers();

	glFlush();

	if (ContextManager)
		return ContextManager->swapBuffers();

	return false;
}

//! Returns the transformation set by setTransform
const core::matrix4 &VideoDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return Matrices[state];
}

//! sets transformation
void VideoDriver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4 &mat)
{
	Matrices[state] = mat;
	Transformation3DChanged = true;
}

bool VideoDriver::uploadHardwareBuffer(OpenGLVBO &vbo,
	const void *buffer, size_t bufferSize, scene::E_HARDWARE_MAPPING hint)
{
	accountHWBufferUpload(bufferSize);

	GLenum usage = GL_STATIC_DRAW;
	if (hint == scene::EHM_STREAM)
		usage = GL_STREAM_DRAW;
	else if (hint == scene::EHM_DYNAMIC)
		usage = GL_DYNAMIC_DRAW;

	vbo.upload(buffer, bufferSize, 0, usage);

	return (!testGLError());
}

bool VideoDriver::updateVertexHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	assert(HWBuffer->IsVertex);
	const auto *vb = HWBuffer->VertexBuffer;
	assert(vb);

	const u32 vertexSize = getVertexPitchFromType(vb->getType());
	const size_t bufferSize = vertexSize * vb->getCount();

	return uploadHardwareBuffer(HWBuffer->Vbo, vb->getData(),
		bufferSize, vb->getHardwareMappingHint());
}

bool VideoDriver::updateIndexHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	assert(!HWBuffer->IsVertex);
	const auto *ib = HWBuffer->IndexBuffer;
	assert(ib);

	u32 indexSize;
	switch (ib->getType()) {
	case EIT_16BIT:
		indexSize = sizeof(u16);
		break;
	case EIT_32BIT:
		indexSize = sizeof(u32);
		break;
	default:
		return false;
	}

	const size_t bufferSize = ib->getCount() * indexSize;

	return uploadHardwareBuffer(HWBuffer->Vbo, ib->getData(),
		bufferSize, ib->getHardwareMappingHint());
}

bool VideoDriver::updateHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return false;

	if (HWBuffer->IsVertex) {
		assert(HWBuffer->VertexBuffer);
		if (HWBuffer->ChangedID != HWBuffer->VertexBuffer->getChangedID() || !HWBuffer->Vbo.exists()) {
			if (!updateVertexHardwareBuffer(HWBuffer))
				return false;
			HWBuffer->ChangedID = HWBuffer->VertexBuffer->getChangedID();
		}
	} else {
		assert(HWBuffer->IndexBuffer);
		if (HWBuffer->ChangedID != HWBuffer->IndexBuffer->getChangedID() || !HWBuffer->Vbo.exists()) {
			if (!updateIndexHardwareBuffer(HWBuffer))
				return false;
			HWBuffer->ChangedID = HWBuffer->IndexBuffer->getChangedID();
		}
	}
	return true;
}

VideoDriver::SHWBufferLink *VideoDriver::createHardwareBuffer(const scene::IVertexBuffer *vb)
{
	if (!vb || vb->getHardwareMappingHint() == scene::EHM_NEVER)
		return 0;

	auto *HWBuffer = new SHWBufferLink(vb);
	registerHardwareBuffer(HWBuffer);

	if (!updateVertexHardwareBuffer(HWBuffer)) {
		deleteHardwareBuffer(HWBuffer);
		return 0;
	}

	return HWBuffer;
}

VideoDriver::SHWBufferLink *VideoDriver::createHardwareBuffer(const scene::IIndexBuffer *ib)
{
	if (!ib || ib->getHardwareMappingHint() == scene::EHM_NEVER)
		return 0;

	auto *HWBuffer = new SHWBufferLink(ib);
	registerHardwareBuffer(HWBuffer);

	if (!updateIndexHardwareBuffer(HWBuffer)) {
		deleteHardwareBuffer(HWBuffer);
		return 0;
	}

	return HWBuffer;
}

void VideoDriver::deleteHardwareBuffer(SHWBufferLink *HWBuffer)
{
	if (!HWBuffer)
		return;

	HWBuffer->Vbo.destroy();

	if (!HWBuffer)
		return;
	const size_t pos = HWBuffer->ListPosition;
	assert(HWBufferList.at(pos) == HWBuffer);
	if (HWBufferList.size() < 2 || pos == HWBufferList.size() - 1) {
		HWBufferList.erase(HWBufferList.begin() + pos);
	} else {
		std::swap(HWBufferList[pos], HWBufferList.back());
		HWBufferList.pop_back();
		HWBufferList[pos]->ListPosition = pos;
	}
	delete HWBuffer;
}

void VideoDriver::drawBuffers(const scene::IVertexBuffer *vb,
	const scene::IIndexBuffer *ib, u32 PrimitiveCount,
	scene::E_PRIMITIVE_TYPE PrimitiveType)
{
	if (!vb || !ib)
		return;

	auto *hwvert = getBufferLink(vb);
	auto *hwidx = getBufferLink(ib);
	updateHardwareBuffer(hwvert);
	updateHardwareBuffer(hwidx);

	const void *vertices = vb->getData();
	if (hwvert) {
		assert(hwvert->IsVertex);
		assert(hwvert->Vbo.exists());
		glBindBuffer(GL_ARRAY_BUFFER, hwvert->Vbo.getName());
		vertices = nullptr;
	}

	const void *indexList = ib->getData();
	if (hwidx) {
		assert(!hwidx->IsVertex);
		assert(hwidx->Vbo.exists());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hwidx->Vbo.getName());
		indexList = nullptr;
	}

	drawVertexPrimitiveList(vertices, vb->getCount(), indexList,
		PrimitiveCount, vb->getType(), PrimitiveType, ib->getType());

	if (hwvert)
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (hwidx)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

RenderTarget *VideoDriver::addRenderTarget()
{
	RenderTarget *renderTarget = new RenderTarget(this);
	RenderTargets.push_back(renderTarget);

	return renderTarget;
}

//! draws a vertex primitive list
void VideoDriver::drawVertexPrimitiveList(const void *vertices, u32 vertexCount,
		const void *indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	if ((iType == EIT_16BIT) && (vertexCount > 65536))
		g_irrlogger->log("Too many vertices for 16bit index type, render artifacts may occur.");
	FrameStats.Drawcalls++;
	FrameStats.PrimitivesDrawn += primitiveCount;

	setRenderStates3DMode();

	drawGeneric(vertices, indexList, primitiveCount, vType, pType, iType);
}

//! draws a vertex primitive list in 2d
void VideoDriver::draw2DVertexPrimitiveList(const void *vertices, u32 vertexCount,
		const void *indexList, u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!vertices)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	if ((iType == EIT_16BIT) && (vertexCount > 65536))
		g_irrlogger->log("Too many vertices for 16bit index type, render artifacts may occur.");
	FrameStats.Drawcalls++;
	FrameStats.PrimitivesDrawn += primitiveCount;

	setRenderStates2DMode(
		Material.MaterialType == EMT_TRANSPARENT_VERTEX_ALPHA,
		Material.getTexture(0),
		Material.MaterialType == EMT_TRANSPARENT_ALPHA_CHANNEL
	);

	drawGeneric(vertices, indexList, primitiveCount, vType, pType, iType);
}

void VideoDriver::draw2DImage(const video::ITexture *texture, const core::position2d<s32> &destPos,
		const core::rect<s32> &sourceRect, const core::rect<s32> *clipRect, SColor color,
		bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	if (!sourceRect.isValid())
		return;

	SColor colors[4] = {color, color, color, color};
	draw2DImage(texture, {destPos, sourceRect.getSize()}, sourceRect, clipRect, colors, useAlphaChannelOfTexture);
}

void VideoDriver::draw2DImage(const video::ITexture *texture, const core::rect<s32> &destRect,
		const core::rect<s32> &sourceRect, const core::rect<s32> *clipRect,
		const video::SColor *const colors, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const core::dimension2du &ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);
	const core::rect<f32> tcoords(
			sourceRect.UpperLeftCorner.X * invW,
			(isRTT ? sourceRect.LowerRightCorner.Y : sourceRect.UpperLeftCorner.Y) * invH,
			sourceRect.LowerRightCorner.X * invW,
			(isRTT ? sourceRect.UpperLeftCorner.Y : sourceRect.LowerRightCorner.Y) * invH);

	const video::SColor temp[4] = {
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
			0xFFFFFFFF,
		};

	const video::SColor *const useColor = colors ? colors : temp;

	chooseMaterial2D();
	if (!setMaterialTexture(0, texture))
		return;

	setRenderStates2DMode(useColor[0].getAlpha() < 255 || useColor[1].getAlpha() < 255 ||
								  useColor[2].getAlpha() < 255 || useColor[3].getAlpha() < 255,
			true, useAlphaChannelOfTexture);

	const core::dimension2d<u32> &renderTargetSize = getCurrentRenderTargetSize();

	if (clipRect) {
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
				clipRect->getWidth(), clipRect->getHeight());
	}

	f32 left  = (f32)destRect.UpperLeftCorner.X;
	f32 right = (f32)destRect.LowerRightCorner.X;
	f32 down  = (f32)destRect.LowerRightCorner.Y;
	f32 top   = (f32)destRect.UpperLeftCorner.Y;

	S3DVertex vertices[4];
	vertices[0] = S3DVertex(left, top, 0, 0, 0, 1, useColor[0], tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
	vertices[1] = S3DVertex(right, top, 0, 0, 0, 1, useColor[3], tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
	vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, useColor[2], tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
	vertices[3] = S3DVertex(left, down, 0, 0, 0, 1, useColor[1], tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

	drawQuad(vt2DImage, vertices);

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);

	testGLError();
}

void VideoDriver::draw2DImage(const video::ITexture *texture, u32 layer, bool flip)
{
	if (!texture)
		return;

	chooseMaterial2D();
	if (!setMaterialTexture(0, texture))
		return;

	setRenderStates2DMode(false, true, true);

	S3DVertex quad2DVertices[4];

	quad2DVertices[0].Pos = core::vector3df(-1.f, 1.f, 0.f);
	quad2DVertices[1].Pos = core::vector3df(1.f, 1.f, 0.f);
	quad2DVertices[2].Pos = core::vector3df(1.f, -1.f, 0.f);
	quad2DVertices[3].Pos = core::vector3df(-1.f, -1.f, 0.f);

	f32 modificator = (flip) ? 1.f : 0.f;

	quad2DVertices[0].TCoords = core::vector2df(0.f, 0.f + modificator);
	quad2DVertices[1].TCoords = core::vector2df(1.f, 0.f + modificator);
	quad2DVertices[2].TCoords = core::vector2df(1.f, 1.f - modificator);
	quad2DVertices[3].TCoords = core::vector2df(0.f, 1.f - modificator);

	quad2DVertices[0].Color = SColor(0xFFFFFFFF);
	quad2DVertices[1].Color = SColor(0xFFFFFFFF);
	quad2DVertices[2].Color = SColor(0xFFFFFFFF);
	quad2DVertices[3].Color = SColor(0xFFFFFFFF);

	drawQuad(vt2DImage, quad2DVertices);
}

void VideoDriver::draw2DImageBatch(const video::ITexture *texture,
		const core::array<core::position2d<s32>> &positions,
		const core::array<core::rect<s32>> &sourceRects,
		const core::rect<s32> *clipRect,
		SColor color, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	chooseMaterial2D();
	if (!setMaterialTexture(0, texture))
		return;

	setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

	const core::dimension2d<u32> &renderTargetSize = getCurrentRenderTargetSize();

	if (clipRect) {
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
				clipRect->getWidth(), clipRect->getHeight());
	}

	const u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());
	assert(6 * drawCount * sizeof(u16) <= QuadIndexVBO.getSize()); // FIXME split the batch? or let it crash?

	std::vector<S3DVertex> vtx;
	vtx.reserve(drawCount * 4);

	// texcoords need to be flipped horizontally for RTTs
	const bool isRTT = texture->isRenderTarget();
	const core::dimension2du ss = texture->getOriginalSize();
	const f32 invW = 1.f / static_cast<f32>(ss.Width);
	const f32 invH = 1.f / static_cast<f32>(ss.Height);

	for (u32 i = 0; i < drawCount; i++) {
		const core::position2d<s32> targetPos = positions[i];
		const core::rect<s32> sourceRect = sourceRects[i];

		// now draw it.

		const core::rect<f32> tcoords(
			sourceRect.UpperLeftCorner.X * invW,
			(isRTT ? sourceRect.LowerRightCorner.Y : sourceRect.UpperLeftCorner.Y) * invH,
			sourceRect.LowerRightCorner.X * invW,
			(isRTT ? sourceRect.UpperLeftCorner.Y : sourceRect.LowerRightCorner.Y) * invH);

		const core::rect<s32> poss(targetPos, sourceRect.getSize());

		f32 left  = (f32)poss.UpperLeftCorner.X;
		f32 right = (f32)poss.LowerRightCorner.X;
		f32 down  = (f32)poss.LowerRightCorner.Y;
		f32 top   = (f32)poss.UpperLeftCorner.Y;

		vtx.emplace_back(left, top, 0.0f,
				0.0f, 0.0f, 0.0f, color,
				tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
		vtx.emplace_back(right, top, 0.0f,
				0.0f, 0.0f, 0.0f, color,
				tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
		vtx.emplace_back(right, down, 0.0f,
				0.0f, 0.0f, 0.0f, color,
				tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
		vtx.emplace_back(left, down, 0.0f,
				0.0f, 0.0f, 0.0f, color,
				tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QuadIndexVBO.getName());
	drawElements(GL_TRIANGLES, vt2DImage, vtx.data(), vtx.size(), 0, 6 * drawCount);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}

//! draw a 2d rectangle
void VideoDriver::draw2DRectangle(SColor color,
		const core::rect<s32> &position,
		const core::rect<s32> *clip)
{
	draw2DRectangle(position, color, color, color, color, clip);
}

//! draw an 2d rectangle
void VideoDriver::draw2DRectangle(const core::rect<s32> &position,
		SColor colorLeftUp, SColor colorRightUp,
		SColor colorLeftDown, SColor colorRightDown,
		const core::rect<s32> *clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	chooseMaterial2D();
	setMaterialTexture(0, 0);

	setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
								  colorRightUp.getAlpha() < 255 ||
								  colorLeftDown.getAlpha() < 255 ||
								  colorRightDown.getAlpha() < 255,
			false, false);

	f32 left  = (f32)pos.UpperLeftCorner.X;
	f32 right = (f32)pos.LowerRightCorner.X;
	f32 down  = (f32)pos.LowerRightCorner.Y;
	f32 top   = (f32)pos.UpperLeftCorner.Y;

	S3DVertex vertices[4];
	vertices[0] = S3DVertex(left,   top, 0, 0, 0, 1, colorLeftUp, 0, 0);
	vertices[1] = S3DVertex(right,  top, 0, 0, 0, 1, colorRightUp, 0, 0);
	vertices[2] = S3DVertex(right, down, 0, 0, 0, 1, colorRightDown, 0, 0);
	vertices[3] = S3DVertex(left,  down, 0, 0, 0, 1, colorLeftDown, 0, 0);

	drawQuad(vtPrimitive, vertices);
}

//! Draws a 2d line.
void VideoDriver::draw2DLine(const core::position2d<s32> &start,
		const core::position2d<s32> &end, SColor color)
{
	{
		chooseMaterial2D();
		setMaterialTexture(0, 0);

		setRenderStates2DMode(color.getAlpha() < 255, false, false);

		f32 startX = (f32)start.X;
		f32 endX   = (f32)end.X;
		f32 startY = (f32)start.Y;
		f32 endY   = (f32)end.Y;

		S3DVertex vertices[2];
		vertices[0] = S3DVertex(startX, startY, 0, 0, 0, 1, color, 0, 0);
		vertices[1] = S3DVertex(endX, endY, 0, 0, 0, 1, color, 1, 1);

		drawArrays(GL_LINES, vtPrimitive, vertices, 2);
	}
}

void VideoDriver::drawQuad(const VertexType &vertexType, const S3DVertex (&vertices)[4])
{
	drawArrays(GL_TRIANGLE_FAN, vertexType, vertices, 4);
}

void VideoDriver::drawArrays(GLenum primitiveType, const VertexType &vertexType, const void *vertices, int vertexCount)
{
	beginDraw(vertexType, reinterpret_cast<uintptr_t>(vertices));
	glDrawArrays(primitiveType, 0, vertexCount);
	endDraw(vertexType);
}

void VideoDriver::drawElements(GLenum primitiveType, const VertexType &vertexType, const void *vertices, int vertexCount, const u16 *indices, int indexCount)
{
	beginDraw(vertexType, reinterpret_cast<uintptr_t>(vertices));
	glDrawRangeElements(primitiveType, 0, vertexCount - 1, indexCount, GL_UNSIGNED_SHORT, indices);
	endDraw(vertexType);
}

void VideoDriver::drawGeneric(const void *vertices, const void *indexList,
		u32 primitiveCount,
		E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	auto &vTypeDesc = getVertexTypeDescription(vType);
	beginDraw(vTypeDesc, reinterpret_cast<uintptr_t>(vertices));
	GLenum indexSize = 0;

	switch (iType) {
	case EIT_16BIT:
		indexSize = GL_UNSIGNED_SHORT;
		break;
	case EIT_32BIT:
		indexSize = GL_UNSIGNED_INT;
		break;
	}

	switch (pType) {
	case scene::EPT_POINTS:
	case scene::EPT_POINT_SPRITES:
		glDrawArrays(GL_POINTS, 0, primitiveCount);
		break;
	case scene::EPT_LINE_STRIP:
		glDrawElements(GL_LINE_STRIP, primitiveCount + 1, indexSize, indexList);
		break;
	case scene::EPT_LINE_LOOP:
		glDrawElements(GL_LINE_LOOP, primitiveCount, indexSize, indexList);
		break;
	case scene::EPT_LINES:
		glDrawElements(GL_LINES, primitiveCount * 2, indexSize, indexList);
		break;
	case scene::EPT_TRIANGLE_STRIP:
		glDrawElements(GL_TRIANGLE_STRIP, primitiveCount + 2, indexSize, indexList);
		break;
	case scene::EPT_TRIANGLE_FAN:
		glDrawElements(GL_TRIANGLE_FAN, primitiveCount + 2, indexSize, indexList);
		break;
	case scene::EPT_TRIANGLES:
		glDrawElements(GL_TRIANGLES, primitiveCount * 3, indexSize, indexList);
		break;
	default:
		break;
	}

	endDraw(vTypeDesc);
}

void VideoDriver::beginDraw(const VertexType &vertexType, uintptr_t verticesBase)
{
	for (auto &attr : vertexType) {
		glEnableVertexAttribArray(attr.Index);
		switch (attr.mode) {
		case VertexAttribute::Mode::Regular:
			glVertexAttribPointer(attr.Index, attr.ComponentCount, attr.ComponentType, GL_FALSE, vertexType.VertexSize, reinterpret_cast<void *>(verticesBase + attr.Offset));
			break;
		case VertexAttribute::Mode::Normalized:
			glVertexAttribPointer(attr.Index, attr.ComponentCount, attr.ComponentType, GL_TRUE, vertexType.VertexSize, reinterpret_cast<void *>(verticesBase + attr.Offset));
			break;
		case VertexAttribute::Mode::Integer:
			glVertexAttribIPointer(attr.Index, attr.ComponentCount, attr.ComponentType, vertexType.VertexSize, reinterpret_cast<void *>(verticesBase + attr.Offset));
			break;
		}
	}
}

void VideoDriver::endDraw(const VertexType &vertexType)
{
	for (auto &attr : vertexType)
		glDisableVertexAttribArray(attr.Index);
}

ITexture *VideoDriver::createDeviceDependentTexture(const io::path &name, E_TEXTURE_TYPE type, const std::vector<IImage*> &images)
{
	return new COpenGLCoreTexture(name, images, type, this);
}

//! prints error if an error happened.
bool VideoDriver::testGLError()
{
	if (!EnableErrorTest)
		return false;

	GLenum g = glGetError();
	const char *err = nullptr;
	switch (g) {
	case GL_NO_ERROR:
		return false;
	case GL_INVALID_ENUM:
		err = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		err = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		err = "GL_INVALID_OPERATION";
		break;
	case GL_STACK_OVERFLOW:
		err = "GL_STACK_OVERFLOW";
		break;
	case GL_STACK_UNDERFLOW:
		err = "GL_STACK_UNDERFLOW";
		break;
	case GL_OUT_OF_MEMORY:
		err = "GL_OUT_OF_MEMORY";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		err = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
#ifdef GL_VERSION_4_5
	case GL_CONTEXT_LOST:
		err = "GL_CONTEXT_LOST";
		break;
#endif
	};

	// Empty the error queue, see <https://www.khronos.org/opengl/wiki/OpenGL_Error>
	bool multiple = false;
	while (glGetError() != GL_NO_ERROR)
		multiple = true;

	const char *file = __FILE__;
	int line = __LINE__;

	// basename
	for (char sep : {'/', '\\'}) {
		const char *tmp = strrchr(file, sep);
		if (tmp)
			file = tmp+1;
	}

	char buf[80];
	snprintf_irr(buf, sizeof(buf), "%s %s:%d%s",
		err, file, line, multiple ? " (older errors exist)" : "");
	g_irrlogger->log(buf, ELL_ERROR);
	return true;
}

const core::dimension2d<u32> &VideoDriver::getCurrentRenderTargetSize() const
{
	if (CurrentRenderTargetSize.Width == 0)
		return ScreenSize;
	else
		return CurrentRenderTargetSize;
}

//! \return Returns the name of the video driver.
const char *VideoDriver::getName() const
{
	return Name.c_str();
}

void VideoDriver::setViewPort(const core::rect<s32> &area)
{
	core::rect<s32> vp = area;
	core::rect<s32> rendert(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
	vp.clipAgainst(rendert);

	if (vp.getHeight() > 0 && vp.getWidth() > 0)
		Context->setViewportSize(
			{vp.UpperLeftCorner.X, (s32)(getCurrentRenderTargetSize().Height - vp.UpperLeftCorner.Y - vp.getHeight()),
			 vp.getWidth(), vp.getHeight()});

	ViewPort = vp;
}

void VideoDriver::setViewPortRaw(u32 width, u32 height)
{
	Context->setViewportSize({0, 0, (s32)width, (s32)height});
	ViewPort = core::recti(0, 0, width, height);
}

//! Draws a 3d line.
void VideoDriver::draw3DLine(const core::vector3df &start,
		const core::vector3df &end, SColor color)
{
	setRenderStates3DMode();

	S3DVertex vertices[2];
	vertices[0] = S3DVertex(start.X, start.Y, start.Z, 0, 0, 1, color, 0, 0);
	vertices[1] = S3DVertex(end.X, end.Y, end.Z, 0, 0, 1, color, 0, 0);

	drawArrays(GL_LINES, vtPrimitive, vertices, 2);
}

//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void VideoDriver::OnResize(const core::dimension2d<u32> &size)
{
	if (ViewPort.getWidth() == (s32)ScreenSize.Width &&
			ViewPort.getHeight() == (s32)ScreenSize.Height)
		ViewPort = core::rect<s32>(core::position2d<s32>(0, 0),
				core::dimension2di(size));

	ScreenSize = size;

	Context->setViewportSize({0, 0, (s32)size.Width, (s32)size.Height});
	Transformation3DChanged = true;
}

//! returns color format
ECOLOR_FORMAT VideoDriver::getColorFormat() const
{
	return ColorFormat;
}

ITexture *VideoDriver::addRenderTargetTexture(const core::dimension2d<u32> &size,
		const io::path &name, const ECOLOR_FORMAT format)
{
	return addRenderTargetTextureMs(size, 0, name, format);
}

ITexture *VideoDriver::addRenderTargetTextureMs(const core::dimension2d<u32> &size, u8 msaa,
		const io::path &name, const ECOLOR_FORMAT format)
{
	// disable mip-mapping
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	COpenGLCoreTexture *renderTargetTexture = new COpenGLCoreTexture(name, size, msaa > 0 ? ETT_2D_MS : ETT_2D, format, this, msaa);
	addTexture(renderTargetTexture);
	renderTargetTexture->drop();

	// restore mip-mapping
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

	return renderTargetTexture;
}

ITexture *VideoDriver::addRenderTargetTextureCubemap(const u32 sideLen, const io::path &name, const ECOLOR_FORMAT format)
{
	// disable mip-mapping
	bool generateMipLevels = getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, false);

	bool supportForFBO = (features.ColorAttachment > 0);

	const core::dimension2d<u32> size(sideLen, sideLen);
	core::dimension2du destSize(size);

	if (!supportForFBO) {
		destSize = core::dimension2d<u32>(core::min_(size.Width, ScreenSize.Width), core::min_(size.Height, ScreenSize.Height));
		destSize = destSize.getOptimalSize((size == size.getOptimalSize()), false, false);
	}

	COpenGLCoreTexture *renderTargetTexture = new COpenGLCoreTexture(name, destSize, ETT_CUBEMAP, format, this);
	addTexture(renderTargetTexture);
	renderTargetTexture->drop();

	// restore mip-mapping
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, generateMipLevels);

	return renderTargetTexture;
}

//! Returns the maximum amount of primitives
u32 VideoDriver::getMaximalPrimitiveCount() const
{
	return Version.Spec == OpenGLSpec::ES ? 65535 : 0x7fffffff;
}

bool VideoDriver::setRenderTargetEx(RenderTarget *target, u16 clearFlag, SColor clearColor, f32 clearDepth, u8 clearStencil)
{
	core::dimension2d<u32> destRenderTargetSize(0, 0);

	if (target) {
		Context->setRenderTarget(target);

		destRenderTargetSize = target->getSize();

		setViewPortRaw(destRenderTargetSize.Width, destRenderTargetSize.Height);
	} else {
		Context->setRenderTarget(nullptr);

		destRenderTargetSize = core::dimension2d<u32>(0, 0);

		setViewPortRaw(ScreenSize.Width, ScreenSize.Height);
	}

	if (CurrentRenderTargetSize != destRenderTargetSize) {
		CurrentRenderTargetSize = destRenderTargetSize;

		Transformation3DChanged = true;
	}

	CurrentRenderTarget = target;

	Context->clearBuffers(clearFlag, clearColor, clearDepth, clearStencil);

	return true;
}

//! Returns an image created from the last rendered frame.
// We want to read the front buffer to get the latest render finished.
// This is not possible under ogl-es, though, so one has to call this method
// outside of the render loop only.
IImage *VideoDriver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
	if (target == video::ERT_MULTI_RENDER_TEXTURES || target == video::ERT_RENDER_TEXTURE || target == video::ERT_STEREO_BOTH_BUFFERS)
		return 0;

	GLint internalformat = GL_RGBA;
	GLint type = GL_UNSIGNED_BYTE;
	{
		//			glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT, &internalformat);
		//			glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE, &type);
		// there's a format we don't support ATM
		if (GL_UNSIGNED_SHORT_4_4_4_4 == type) {
			internalformat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
		}
	}

	IImage *newImage = 0;
	if (GL_RGBA == internalformat) {
		if (GL_UNSIGNED_BYTE == type)
			newImage = new CImage(ECF_A8R8G8B8, ScreenSize);
		else
			newImage = new CImage(ECF_A1R5G5B5, ScreenSize);
	} else {
		if (GL_UNSIGNED_BYTE == type)
			newImage = new CImage(ECF_R8G8B8, ScreenSize);
		else
			newImage = new CImage(ECF_R5G6B5, ScreenSize);
	}

	if (!newImage)
		return 0;

	u8 *pixels = static_cast<u8 *>(newImage->getData());
	if (!pixels) {
		newImage->drop();
		return 0;
	}

	glReadPixels(0, 0, ScreenSize.Width, ScreenSize.Height, internalformat, type, pixels);
	testGLError();

	// opengl images are horizontally flipped, so we have to fix that here.
	const s32 pitch = newImage->getPitch();
	u8 *p2 = pixels + (ScreenSize.Height - 1) * pitch;
	u8 *tmpBuffer = new u8[pitch];
	for (u32 i = 0; i < ScreenSize.Height; i += 2) {
		memcpy(tmpBuffer, pixels, pitch);
		memcpy(pixels, p2, pitch);
		memcpy(p2, tmpBuffer, pitch);
		pixels += pitch;
		p2 -= pitch;
	}
	delete[] tmpBuffer;

	// also GL_RGBA doesn't match the internal encoding of the image (which is BGRA)
	if (GL_RGBA == internalformat && GL_UNSIGNED_BYTE == type) {
		pixels = static_cast<u8 *>(newImage->getData());
		for (u32 i = 0; i < ScreenSize.Height; i++) {
			for (u32 j = 0; j < ScreenSize.Width; j++) {
				u32 c = *(u32 *)(pixels + 4 * j);
				*(u32 *)(pixels + 4 * j) = (c & 0xFF00FF00) |
										   ((c & 0x00FF0000) >> 16) | ((c & 0x000000FF) << 16);
			}
			pixels += pitch;
		}
	}

	if (testGLError()) {
		newImage->drop();
		return 0;
	}
	return newImage;
}

void VideoDriver::removeTexture(ITexture *texture)
{
	Context->removeTexture(texture);

	if (!texture)
		return;
	SSurface s;
	s.Surface = texture;

	s32 last;
	s32 first = Textures.binary_search_multi(s, last);
	if (first == -1)
		return;
	for (u32 i = first; i <= (u32)last; i++) {
		if (Textures[i].Surface == texture) {
			texture->drop();
			Textures.erase(i);
			return;
		}
	}
}

core::dimension2du VideoDriver::getMaxTextureSize() const
{
	return core::dimension2du(features.MaxTextureSize, features.MaxTextureSize);
}

bool VideoDriver::needsTransparentRenderPass(const video::SMaterial &material) const
{
	video::IMaterialRenderer *rnd = getMaterialRenderer(material.MaterialType);
	if (rnd && rnd->isTransparent())
		return true;

	return material.isAlphaBlendOperation();
}

DrawContext *VideoDriver::getContext() const
{
	return Context.get();
}

VideoDriver::SHWBufferLink *VideoDriver::getBufferLink(const scene::IVertexBuffer *vb)
{
	if (!vb || !isHardwareBufferRecommend(vb))
		return 0;

	SHWBufferLink *HWBuffer = reinterpret_cast<SHWBufferLink *>(vb->getHWBuffer());
	if (HWBuffer)
		return HWBuffer;

	return createHardwareBuffer(vb);
}

VideoDriver::SHWBufferLink *VideoDriver::getBufferLink(const scene::IIndexBuffer *ib)
{
	if (!ib || !isHardwareBufferRecommend(ib))
		return 0;

	SHWBufferLink *HWBuffer = reinterpret_cast<SHWBufferLink *>(ib->getHWBuffer());
	if (HWBuffer)
		return HWBuffer;

	return createHardwareBuffer(ib);
}

void VideoDriver::registerHardwareBuffer(SHWBufferLink *HWBuffer)
{
	assert(HWBuffer);
	HWBuffer->ListPosition = HWBufferList.size();
	HWBufferList.push_back(HWBuffer);
}

void VideoDriver::expireHardwareBuffers()
{
	for (size_t i = 0; i < HWBufferList.size(); ) {
		auto *Link = HWBufferList[i];

		bool del;
		if (Link->IsVertex)
			del = !Link->VertexBuffer || Link->VertexBuffer->getReferenceCount() == 1;
		else
			del = !Link->IndexBuffer || Link->IndexBuffer->getReferenceCount() == 1;
		if (del)
			deleteHardwareBuffer(Link);
		else
			i++;
	}

	FrameStats.HWBuffersActive = HWBufferList.size();
}

void VideoDriver::deleteAllTextures()
{
	setMaterial(SMaterial());

	for (u32 i = 0; i < RenderTargets.size(); ++i)
		RenderTargets[i]->setTextures({nullptr}, nullptr);

	for (u32 i = 0; i < Textures.size(); ++i)
		Textures[i].Surface->drop();

	Textures.clear();

	SharedDepthTextures.clear();
}

void VideoDriver::removeRenderTarget(RenderTarget *renderTarget)
{
	if (!renderTarget)
		return;

	for (u32 i = 0; i < RenderTargets.size(); ++i) {
		if (RenderTargets[i] == renderTarget) {
			RenderTargets[i]->drop();
			RenderTargets.erase(i);
			return;
		}
	}
}

void VideoDriver::removeAllRenderTargets()
{
	for (u32 i = 0; i < RenderTargets.size(); ++i)
		RenderTargets[i]->drop();

	RenderTargets.clear();

	SharedRenderTarget = nullptr;
}

void VideoDriver::removeAllHardwareBuffers()
{
	while (!HWBufferList.empty())
		deleteHardwareBuffer(HWBufferList.front());
}

bool VideoDriver::isHardwareBufferRecommend(const scene::IVertexBuffer *vb)
{
	if (!vb || vb->getHardwareMappingHint() == scene::EHM_NEVER)
		return false;

	if (vb->getCount() < MinVertexCountForVBO)
		return false;

	return true;
}

bool VideoDriver::isHardwareBufferRecommend(const scene::IIndexBuffer *ib)
{
	if (!ib || ib->getHardwareMappingHint() == scene::EHM_NEVER)
		return false;

	if (ib->getCount() < MinVertexCountForVBO * 3)
		return false;

	return true;
}

void VideoDriver::addTexture(ITexture *texture)
{
	if (texture) {
		SSurface s;
		s.Surface = texture;
		texture->grab();

		Textures.push_back(s);
	}
}

void VideoDriver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
	if (enabled && ((flag == ETCF_ALWAYS_16_BIT) || (flag == ETCF_ALWAYS_32_BIT) || (flag == ETCF_OPTIMIZED_FOR_QUALITY) || (flag == ETCF_OPTIMIZED_FOR_SPEED))) {
		setTextureCreationFlag(ETCF_ALWAYS_16_BIT, false);
		setTextureCreationFlag(ETCF_ALWAYS_32_BIT, false);
		setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, false);
		setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, false);
	}

	if (enabled)
		TextureCreationFlags |= flag;
	else
		TextureCreationFlags &= ~flag;
}

bool VideoDriver::getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag) const
{
	return (TextureCreationFlags & flag) != 0;
}

void VideoDriver::setFog(SColor color, E_FOG_TYPE fogType, f32 start, f32 end, f32 density, bool pixelFog, bool rangeFog)
{
	FogColor = color;
	FogType = fogType;
	FogStart = start;
	FogEnd = end;
	FogDensity = density;
	PixelFog = pixelFog;
	RangeFog = rangeFog;
}

void VideoDriver::getFog(SColor &color, E_FOG_TYPE &fogType, f32 &start, f32 &end, f32 &density, bool &pixelFog, bool &rangeFog)
{
	color = FogColor;
	fogType = FogType;
	start = FogStart;
	end = FogEnd;
	density = FogDensity;
	pixelFog = PixelFog;
	rangeFog = RangeFog;
}

scene::IMeshManipulator *VideoDriver::getMeshManipulator()
{
	return MeshManipulator;
}

bool VideoDriver::checkPrimitiveCount(u32 prmCount) const
{
	const u32 m = getMaximalPrimitiveCount();

	if (prmCount > m) {
		char tmp[128];
		snprintf_irr(tmp, sizeof(tmp), "Could not draw triangles, too many primitives(%u), maximum is %u.", prmCount, m);
		g_irrlogger->log(tmp, ELL_ERROR);
		return false;
	}

	return true;
}

} // end namespace
