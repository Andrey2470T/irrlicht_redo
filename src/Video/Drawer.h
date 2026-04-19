#pragma once

#include "Mesh/IMeshBuffer.h"
#include "Mesh/VertexTypes.h"
#include "Mesh/SVertexIndex.h"
#include "Utils/matrix4.h"
#include "Video/DrawContext.h"
#include "Video/Texture.h"
#include "Utils/irrArray.h"
#include <memory>
#include <optional>

namespace scene
{
struct VertexDescriptor;
}

namespace video
{

class VideoDriver;
class HWBuffer;

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

class Drawer
{
	VideoDriver *Driver;

public:
	Drawer(VideoDriver *_Driver)
		: Driver(_Driver)
	{}

	void drawMeshBuffer(const scene::IMeshBuffer *mb, std::optional<scene::IIndexBuffer *> indices=std::nullopt);

	void drawMeshBufferNormals(const scene::IMeshBuffer *mb, f32 length = 10.f,
		SColor color = 0xffffffff);

	//! draws a vertex primitive list
	void drawVertexPrimitiveList(const void *vertices, u32 vertexCount,
			const void *indexList, u32 primitiveCount,
			scene::E_VERTEX_TYPE vType = scene::EVT_3D, scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES,
			E_INDEX_TYPE iType = EIT_16BIT);

	//! draws a vertex primitive list in 2d
	void draw2DVertexPrimitiveList(const void *vertices, u32 vertexCount,
			const void *indexList, u32 primitiveCount,
			scene::E_VERTEX_TYPE vType = scene::EVT_3D, scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES,
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
	void drawIndexedTriangleList(const scene::Vertex3D *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, scene::EVT_3D, scene::EPT_TRIANGLES, EIT_16BIT);
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
	void drawIndexedTriangleList(const scene::Vertex2TCoords *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, scene::EVT_2TCOORDS, scene::EPT_TRIANGLES, EIT_16BIT);
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
	void drawIndexedTriangleList(const scene::VertexTangents *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, scene::EVT_TANGENTS, scene::EPT_TRIANGLES, EIT_16BIT);
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
	void drawIndexedTriangleFan(const scene::Vertex3D *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, scene::EVT_3D, scene::EPT_TRIANGLE_FAN, EIT_16BIT);
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
	void drawIndexedTriangleFan(const scene::Vertex2TCoords *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, scene::EVT_2TCOORDS, scene::EPT_TRIANGLE_FAN, EIT_16BIT);
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
	void drawIndexedTriangleFan(const scene::VertexTangents *vertices,
			u32 vertexCount, const u16 *indexList, u32 triangleCount)
	{
		drawVertexPrimitiveList(vertices, vertexCount, indexList, triangleCount, scene::EVT_TANGENTS, scene::EPT_TRIANGLE_FAN, EIT_16BIT);
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

	void enableAttributeArrays(const scene::VertexDescriptor &vertexDesc, uintptr_t verticesBase);
	void disableAttributeArrays(const scene::VertexDescriptor &vertexDesc);

protected:
	void initQuadsIndices(u32 max_vertex_count = 65536);
	void destroyQuadIndices();

	SFrameStats FrameStats;

private:
	void drawQuad(const scene::Vertex2D (&vertices)[4]);
	void drawArrays(scene::E_PRIMITIVE_TYPE primitiveType, const scene::VertexDescriptor &vertexDesc, const void *vertices, int vertexCount);
	void drawElements(scene::E_PRIMITIVE_TYPE primitiveType, const scene::VertexDescriptor &vertexDesc, const void *vertices, int vertexCount, const u16 *indices, int indexCount);
	void drawElements(scene::E_PRIMITIVE_TYPE primitiveType, const scene::VertexDescriptor &vertexDesc, uintptr_t vertices, uintptr_t indices, int indexCount);

	void drawGeneric(const void *indexList, u32 primitiveCount, scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType);

	bool checkPrimitiveCount(u32 prmcnt) const;

	std::unique_ptr<HWBuffer> QuadIndexVBO;
};

}
