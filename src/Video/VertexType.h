#include "Common.h"
#include "Mesh/S3DVertex.h"
#include "Enums/EVertexAttributes.h"

namespace video
{

struct VertexAttribute
{
	enum Mode : u8
	{
		Regular,
		Normalized,
		Integer,
	};
	u8 Index;
	u8 ComponentCount;
	GLenum ComponentType;
	Mode mode;
	u32 Offset;
};

struct VertexType
{
	u32 VertexSize;
	std::vector<VertexAttribute> Attributes;

	// allow ranged for loops
	inline auto begin() const { return Attributes.begin(); }
	inline auto end() const { return Attributes.end(); }
};

static const VertexType vtStandard = {
		sizeof(S3DVertex),
		{
				{EVA_POSITION, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex, Pos)},
				{EVA_NORMAL, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex, Normal)},
				{EVA_COLOR, 4, GL_UNSIGNED_BYTE, VertexAttribute::Mode::Normalized, offsetof(S3DVertex, Color)},
				{EVA_TCOORD0, 2, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex, TCoords)},
		},
};

// FIXME: this is actually UB because these vertex classes are not "standard-layout"
// they violate the following requirement:
// - only one class in the hierarchy has non-static data members
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winvalid-offsetof"

static const VertexType vt2TCoords = {
		sizeof(S3DVertex2TCoords),
		{
				{EVA_POSITION, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex2TCoords, Pos)},
				{EVA_NORMAL, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex2TCoords, Normal)},
				{EVA_COLOR, 4, GL_UNSIGNED_BYTE, VertexAttribute::Mode::Normalized, offsetof(S3DVertex2TCoords, Color)},
				{EVA_TCOORD0, 2, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex2TCoords, TCoords)},
				{EVA_TCOORD1, 2, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex2TCoords, TCoords2)},
		},
};

static const VertexType vtTangents = {
		sizeof(S3DVertexTangents),
		{
				{EVA_POSITION, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertexTangents, Pos)},
				{EVA_NORMAL, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertexTangents, Normal)},
				{EVA_COLOR, 4, GL_UNSIGNED_BYTE, VertexAttribute::Mode::Normalized, offsetof(S3DVertexTangents, Color)},
				{EVA_TCOORD0, 2, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertexTangents, TCoords)},
				{EVA_TANGENT, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertexTangents, Tangent)},
				{EVA_BINORMAL, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertexTangents, Binormal)},
		},
};

#pragma GCC diagnostic pop

static const VertexType &getVertexTypeDescription(E_VERTEX_TYPE type)
{
	switch (type) {
	case EVT_STANDARD:
		return vtStandard;
	case EVT_2TCOORDS:
		return vt2TCoords;
	case EVT_TANGENTS:
		return vtTangents;
	default:
		IRR_CODE_UNREACHABLE();
	}
}

static const VertexType vt2DImage = {
		sizeof(S3DVertex),
		{
				{EVA_POSITION, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex, Pos)},
				{EVA_COLOR, 4, GL_UNSIGNED_BYTE, VertexAttribute::Mode::Normalized, offsetof(S3DVertex, Color)},
				{EVA_TCOORD0, 2, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex, TCoords)},
		},
};

static const VertexType vtPrimitive = {
		sizeof(S3DVertex),
		{
				{EVA_POSITION, 3, GL_FLOAT, VertexAttribute::Mode::Regular, offsetof(S3DVertex, Pos)},
				{EVA_COLOR, 4, GL_UNSIGNED_BYTE, VertexAttribute::Mode::Normalized, offsetof(S3DVertex, Color)},
		},
};

}
