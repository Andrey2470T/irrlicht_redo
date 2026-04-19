#include "Drawer.h"
#include "Video/VideoDriver.h"
#include "Common.h"
#include "MaterialSystem.h"
#include "Video/HWBuffer.h"
#include "Mesh/IIndexBuffer.h"
#include "GLSpecificInfo.h"

namespace video
{

void Drawer::drawMeshBuffer(const scene::IMeshBuffer *mb, std::optional<scene::IIndexBuffer *> indices)
{
	if (!mb)
		return;

	FrameStats.HWBuffersUploaded += mb->reload(Driver);

	u32 primitiveCount = mb->getPrimitiveCount();
	u32 vertexCount = mb->getVertexCount();
	if (!primitiveCount || !vertexCount)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	if (vertexCount > 65536)
		g_irrlogger->log("Too many vertices for 16bit index type, render artifacts may occur.");
	FrameStats.Drawcalls++;
	FrameStats.PrimitivesDrawn += primitiveCount;

	Driver->setRenderStates3DMode();

	mb->bind();

	drawGeneric((void*)0, primitiveCount, mb->getPrimitiveType(), EIT_16BIT);

	mb->unbind();
}

//! Draws the normals of a mesh buffer
void Drawer::drawMeshBufferNormals(const scene::IMeshBuffer *mb, f32 length, SColor color)
{
	const u32 count = mb->getVertexCount();
	for (u32 i = 0; i < count; ++i) {
		core::vector3df normal = mb->getNormal(i);
		const core::vector3df &pos = mb->getPosition(i);
		draw3DLine(pos, pos + (normal * length), color);
	}
}

//! draws a vertex primitive list
void Drawer::drawVertexPrimitiveList(const void *vertices, u32 vertexCount,
		const void *indexList, u32 primitiveCount,
		scene::E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	if ((iType == EIT_16BIT) && (vertexCount > 65536))
		g_irrlogger->log("Too many vertices for 16bit index type, render artifacts may occur.");
	Driver->FrameStats.Drawcalls++;
	Driver->FrameStats.PrimitivesDrawn += primitiveCount;

	Driver->setRenderStates3DMode();

	auto &vTypeDesc = getVertexTypeDescription(vType);
	enableAttributeArrays(vTypeDesc, reinterpret_cast<uintptr_t>(vertices));

	drawGeneric(indexList, primitiveCount, pType, iType);

	disableAttributeArrays(vTypeDesc);
}

//! draws a vertex primitive list in 2d
void Drawer::draw2DVertexPrimitiveList(const void *vertices, u32 vertexCount,
		const void *indexList, u32 primitiveCount,
		scene::E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if (!primitiveCount || !vertexCount)
		return;

	if (!vertices)
		return;

	if (!checkPrimitiveCount(primitiveCount))
		return;

	if ((iType == EIT_16BIT) && (vertexCount > 65536))
		g_irrlogger->log("Too many vertices for 16bit index type, render artifacts may occur.");
	Driver->FrameStats.Drawcalls++;
	Driver->FrameStats.PrimitivesDrawn += primitiveCount;

	Driver->setRenderStates2DMode(
		Driver->Material.MaterialType == EMT_TRANSPARENT_VERTEX_ALPHA,
		Driver->Material.getTexture(0),
		Driver->Material.MaterialType == EMT_TRANSPARENT_ALPHA_CHANNEL
	);

	auto &vTypeDesc = getVertexTypeDescription(vType);
	enableAttributeArrays(vTypeDesc, reinterpret_cast<uintptr_t>(vertices));

	drawGeneric(indexList, primitiveCount, pType, iType);

	disableAttributeArrays(vTypeDesc);
}

//! draws an 2d image
void Drawer::draw2DImage(const GLTexture *texture, const core::position2d<s32> &destPos, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	draw2DImage(texture, destPos, core::rect<s32>(core::position2d<s32>(0, 0), core::dimension2di(texture->getOriginalSize())),
			0,
			SColor(255, 255, 255, 255),
			useAlphaChannelOfTexture);
}

void Drawer::draw2DImage(const GLTexture *texture, const core::position2d<s32> &destPos,
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

void Drawer::draw2DImage(const GLTexture *texture, const core::rect<s32> &destRect,
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

	Driver->chooseMaterial2D();
	if (!Driver->setMaterialTexture(0, texture))
		return;

	Driver->setRenderStates2DMode(useColor[0].getAlpha() < 255 || useColor[1].getAlpha() < 255 ||
								  useColor[2].getAlpha() < 255 || useColor[3].getAlpha() < 255,
			true, useAlphaChannelOfTexture);

	const core::dimension2d<u32> &renderTargetSize = Driver->getCurrentRenderTargetSize();

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

	scene::Vertex2D vertices[4];
	vertices[0] = {{left, top}, useColor[0], {tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y}};
	vertices[1] = {{right, top}, useColor[3], {tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y}};
	vertices[2] = {{right, down}, useColor[2], {tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y}};
	vertices[3] = {{left, down}, useColor[1], {tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y}};

	drawQuad(vertices);

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);

	TEST_GL_ERROR(Driver);
}

void Drawer::draw2DImage(const GLTexture *texture, u32 layer, bool flip)
{
	if (!texture)
		return;

	Driver->chooseMaterial2D();
	if (!Driver->setMaterialTexture(0, texture))
		return;

	Driver->setRenderStates2DMode(false, true, true);

	scene::Vertex2D quad2DVertices[4];

	quad2DVertices[0].Pos = core::vector2df(-1.f, 1.f);
	quad2DVertices[1].Pos = core::vector2df(1.f, 1.f);
	quad2DVertices[2].Pos = core::vector2df(1.f, -1.f);
	quad2DVertices[3].Pos = core::vector2df(-1.f, -1.f);

	f32 modificator = (flip) ? 1.f : 0.f;

	quad2DVertices[0].TCoords = core::vector2df(0.f, 0.f + modificator);
	quad2DVertices[1].TCoords = core::vector2df(1.f, 0.f + modificator);
	quad2DVertices[2].TCoords = core::vector2df(1.f, 1.f - modificator);
	quad2DVertices[3].TCoords = core::vector2df(0.f, 1.f - modificator);

	quad2DVertices[0].Color = SColor(0xFFFFFFFF);
	quad2DVertices[1].Color = SColor(0xFFFFFFFF);
	quad2DVertices[2].Color = SColor(0xFFFFFFFF);
	quad2DVertices[3].Color = SColor(0xFFFFFFFF);

	drawQuad(quad2DVertices);
}

void Drawer::draw2DImageBatch(const GLTexture *texture,
		const core::array<core::position2d<s32>> &positions,
		const core::array<core::rect<s32>> &sourceRects,
		const core::rect<s32> *clipRect,
		SColor color, bool useAlphaChannelOfTexture)
{
	if (!texture)
		return;

	Driver->chooseMaterial2D();
	if (!Driver->setMaterialTexture(0, texture))
		return;

	Driver->setRenderStates2DMode(color.getAlpha() < 255, true, useAlphaChannelOfTexture);

	const core::dimension2d<u32> &renderTargetSize = Driver->getCurrentRenderTargetSize();

	if (clipRect) {
		if (!clipRect->isValid())
			return;

		glEnable(GL_SCISSOR_TEST);
		glScissor(clipRect->UpperLeftCorner.X, renderTargetSize.Height - clipRect->LowerRightCorner.Y,
				clipRect->getWidth(), clipRect->getHeight());
	}

	const u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());
	assert(6 * drawCount * sizeof(u16) <= QuadIndexVBO->getSize()); // FIXME split the batch? or let it crash?

	std::vector<scene::Vertex2D> vtx;
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

		vtx.push_back({{left, top}, color,
			{tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y}});
		vtx.push_back({{right, top}, color,
			{tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y}});
		vtx.push_back({{right, down}, color,
			{tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y}});
		vtx.push_back({{left, down}, color,
			{tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y}});
	}

	QuadIndexVBO->bind();
	drawElements(scene::EPT_TRIANGLES, scene::Vertex2D::FORMAT, vtx.data(), vtx.size(), 0, 6 * drawCount);
	QuadIndexVBO->unbind();

	if (clipRect)
		glDisable(GL_SCISSOR_TEST);
}

//! draw a 2d rectangle
void Drawer::draw2DRectangle(SColor color,
		const core::rect<s32> &position,
		const core::rect<s32> *clip)
{
	draw2DRectangle(position, color, color, color, color, clip);
}

//! draw an 2d rectangle
void Drawer::draw2DRectangle(const core::rect<s32> &position,
		SColor colorLeftUp, SColor colorRightUp,
		SColor colorLeftDown, SColor colorRightDown,
		const core::rect<s32> *clip)
{
	core::rect<s32> pos = position;

	if (clip)
		pos.clipAgainst(*clip);

	if (!pos.isValid())
		return;

	Driver->chooseMaterial2D();
	Driver->setMaterialTexture(0, 0);

	Driver->setRenderStates2DMode(colorLeftUp.getAlpha() < 255 ||
								  colorRightUp.getAlpha() < 255 ||
								  colorLeftDown.getAlpha() < 255 ||
								  colorRightDown.getAlpha() < 255,
			false, false);

	f32 left  = (f32)pos.UpperLeftCorner.X;
	f32 right = (f32)pos.LowerRightCorner.X;
	f32 down  = (f32)pos.LowerRightCorner.Y;
	f32 top   = (f32)pos.UpperLeftCorner.Y;

	scene::Vertex2D vertices[4];
	vertices[0] = {{left, top}, colorLeftUp, {0, 0}};
	vertices[1] = {{right, top}, colorRightUp, {0, 0}};
	vertices[2] = {{right, down}, colorRightDown, {0, 0}};
	vertices[3] = {{left, down}, colorLeftDown, {0, 0}};

	drawQuad(vertices);
}

//! Draws a 2d line.
void Drawer::draw2DLine(const core::position2d<s32> &start,
		const core::position2d<s32> &end, SColor color)
{
	{
		Driver->chooseMaterial2D();
		Driver->setMaterialTexture(0, 0);

		Driver->setRenderStates2DMode(color.getAlpha() < 255, false, false);

		f32 startX = (f32)start.X;
		f32 endX   = (f32)end.X;
		f32 startY = (f32)start.Y;
		f32 endY   = (f32)end.Y;

		scene::Vertex2D vertices[2];
		vertices[0] = {{startX, startY}, color, {0, 0}};
		vertices[1] = {{endX, endY}, color, {1, 1}};

		drawArrays(scene::EPT_LINES, scene::Vertex2D::FORMAT, vertices, 2);
	}
}

//! Draws a 3d line.
void Drawer::draw3DLine(const core::vector3df &start,
		const core::vector3df &end, SColor color)
{
	Driver->setRenderStates3DMode();

	scene::Vertex3D vertices[2];
	vertices[0] = {{start.X, start.Y, start.Z}, {0, 0, 1}, color, {0, 0}};
	vertices[1] = {{end.X, end.Y, end.Z}, {0, 0, 1}, color, {0, 0}};

	drawArrays(scene::EPT_LINES, scene::Vertex2D::FORMAT, vertices, 2);
}

//! Draws a 3d axis aligned box.
void Drawer::draw3DBox(const core::aabbox3d<f32> &box, SColor color)
{
	core::vector3df edges[8];
	box.getEdges(edges);

	scene::Vertex3D v[8];
	for (u32 i = 0; i < 8; i++) {
		v[i].Pos = edges[i];
		v[i].Color = color;
	}

	const static u16 box_indices[24] = {
		5, 1, 1, 3, 3, 7, 7, 5, 0, 2, 2, 6, 6, 4, 4, 0, 1, 0, 3, 2, 7, 6, 5, 4
	};

	drawVertexPrimitiveList(v, 8, box_indices, 12, scene::EVT_3D, scene::EPT_LINES, EIT_16BIT);
}

void Drawer::drawQuad(const scene::Vertex2D (&vertices)[4])
{
	drawArrays(scene::EPT_TRIANGLE_FAN, scene::Vertex2D::FORMAT, vertices, 4);
}

std::array<GLenum, scene::EPT_COUNT> toGLPrimType = {
	GL_POINTS,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_LINES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	GL_TRIANGLES,
	GL_POINTS
};

void Drawer::drawArrays(scene::E_PRIMITIVE_TYPE primitiveType, const scene::VertexDescriptor &vertexDesc, const void *vertices, int vertexCount)
{
	enableAttributeArrays(vertexDesc, reinterpret_cast<uintptr_t>(vertices));
	glDrawArrays(toGLPrimType[primitiveType], 0, vertexCount);
	disableAttributeArrays(vertexDesc);
}

void Drawer::drawElements(scene::E_PRIMITIVE_TYPE primitiveType, const scene::VertexDescriptor &vertexDesc, const void *vertices, int vertexCount, const u16 *indices, int indexCount)
{
	enableAttributeArrays(vertexDesc, reinterpret_cast<uintptr_t>(vertices));
	glDrawRangeElements(toGLPrimType[primitiveType], 0, vertexCount - 1, indexCount, GL_UNSIGNED_SHORT, indices);
	disableAttributeArrays(vertexDesc);
}

void Drawer::drawGeneric(
	const void *indexList, u32 primitiveCount,
	scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
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
}

std::array<GLenum, (u8)scene::VertexAttribute::Type::COUNT> toGLType = {
	GL_FLOAT,
	GL_UNSIGNED_BYTE,
	GL_INT
};

void Drawer::enableAttributeArrays(const scene::VertexDescriptor &vertexDesc, uintptr_t verticesBase)
{
	for (auto &attr : vertexDesc.Attributes) {
		glEnableVertexAttribArray(attr.Index);
		switch (attr.mode) {
		case scene::VertexAttribute::Mode::REGULAR:
			glVertexAttribPointer(attr.Index, attr.Count, toGLType[(u8)attr.Type], GL_FALSE, vertexDesc.Size, reinterpret_cast<void *>(verticesBase + attr.Offset));
			break;
		case scene::VertexAttribute::Mode::NORMALIZED:
			glVertexAttribPointer(attr.Index, attr.Count, toGLType[(u8)attr.Type], GL_TRUE, vertexDesc.Size, reinterpret_cast<void *>(verticesBase + attr.Offset));
			break;
		case scene::VertexAttribute::Mode::INTEGER:
			glVertexAttribIPointer(attr.Index, attr.Count, toGLType[(u8)attr.Type], vertexDesc.Size, reinterpret_cast<void *>(verticesBase + attr.Offset));
			break;
		}
	}
}

void Drawer::disableAttributeArrays(const scene::VertexDescriptor &vertexDesc)
{
	for (auto &attr : vertexDesc.Attributes)
		glDisableVertexAttribArray(attr.Index);
}

void Drawer::initQuadsIndices(u32 max_vertex_count)
{
	QuadIndexVBO = std::make_unique<HWBuffer>(HWBT_INDEX);

	u32 max_quad_count = max_vertex_count / 4;
	u32 indices_size = 6 * max_quad_count;
	if (indices_size == QuadIndexVBO->getSize() * sizeof(u16))
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
	QuadIndexVBO->upload(QuadsIndices.data(), QuadsIndices.size() * sizeof(u16),
		0, scene::EHM_STATIC, true);
	assert(QuadIndexVBO->exists());
}

void Drawer::destroyQuadIndices()
{
	QuadIndexVBO->destroy();
}

bool Drawer::checkPrimitiveCount(u32 prmCount) const
{
	const u32 m = Driver->GLInfo->getOpenGLVersion().Spec == OpenGLSpec::ES ? 65535 : 0x7fffffff;

	if (prmCount > m) {
		char tmp[128];
		snprintf_irr(tmp, sizeof(tmp), "Could not draw triangles, too many primitives(%u), maximum is %u.", prmCount, m);
		g_irrlogger->log(tmp, ELL_ERROR);
		return false;
	}

	return true;
}

}
