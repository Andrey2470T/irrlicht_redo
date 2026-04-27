#include "Mesh/VertexTypes.h"

namespace scene
{

// The simple 'offsetof' gives out the warning about the non-standard layout for the derivative types
template<typename T, typename M>
static inline u32 get_offset(M T::* member) {
	return static_cast<u32>(reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*member)));
}

const VertexDescriptor Vertex3D::FORMAT = {
	sizeof(Vertex3D),
	{
		{"inPosition", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3D::Pos)},
		{"inNormal", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3D::Normal)},
		{"inColor", 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex3D::Color)},
		{"inTexCoord0", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3D::TCoords)}
	},
};

const VertexDescriptor Vertex2TCoords::FORMAT = {
	sizeof(Vertex2TCoords),
	{
		{"inPosition", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::Pos)},
		{"inNormal", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::Normal)},
		{"inColor", 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex2TCoords::Color)},
		{"inTexCoord0", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::TCoords)},
		{"inTexCoord1", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::TCoords2)}
	},
};

const VertexDescriptor VertexTangents::FORMAT = {
	sizeof(VertexTangents),
	{
		{"inPosition", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Pos)},
		{"inNormal", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Normal)},
		{"inColor", 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&VertexTangents::Color)},
		{"inTexCoord0", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::TCoords)},
		{"inTagent", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Tangent)},
		{"inBinormal", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Binormal)}
	},
};

const VertexDescriptor Vertex2D::FORMAT = {
	sizeof(Vertex2D),
	{
		{"inPosition", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2D::Pos)},
		{"inColor", 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex2D::Color)},
		{"inTexCoord0", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2D::TCoords)}
	},
};

const VertexDescriptor Vertex3DExt::FORMAT = {
	sizeof(Vertex3DExt),
	{
		{"inPosition", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::Pos)},
		{"inNormal", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::Normal)},
		{"inColor", 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex3DExt::Color)},
		{"inTexCoord0", 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::TCoords)},
		{"inAux", 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::Aux)}
	},
};

const VertexDescriptor &getVertexTypeDescription(E_VERTEX_TYPE type)
{
	switch (type) {
	case EVT_3D:
		return Vertex3D::FORMAT;
	case EVT_2TCOORDS:
		return Vertex2TCoords::FORMAT;
	case EVT_TANGENTS:
		return VertexTangents::FORMAT;
	case EVT_2D:
	  return Vertex2D::FORMAT;
	case EVT_3D_EXT:
	  return Vertex3DExt::FORMAT;
	default:
		return Vertex3D::FORMAT;
	}
}

u32 getVertexTypeSize(E_VERTEX_TYPE type)
{
	return getVertexTypeDescription(type).Size;

}

bool operator==(const Vertex3D &a, const Vertex3D &b)
{
	return ((a.Pos == b.Pos) &&
		(a.Normal == b.Normal) &&
		(a.Color == b.Color) &&
		(a.TCoords == b.TCoords));
}
bool operator<(const Vertex3D &a, const Vertex3D &b)
{
	return ((a.Pos < b.Pos) ||
		((a.Pos == b.Pos) && (a.Normal < b.Normal)) ||
		((a.Pos == b.Pos) && (a.Normal == b.Normal) && (a.Color < b.Color)) ||
		((a.Pos == b.Pos) && (a.Normal == b.Normal) && (a.Color == b.Color) && (a.TCoords < b.TCoords)));
}

bool operator==(const VertexAttribute &attr1, const VertexAttribute &attr2)
{
	return (attr1.Name == attr2.Name && attr1.Count == attr2.Count &&
		attr1.Type == attr2.Type && attr1.mode == attr2.mode && attr1.Offset == attr2.Offset);
}
bool operator==(const VertexDescriptor &desc1, const VertexDescriptor &desc2)
{
	return (desc1.Size == desc2.Size && desc1.Attributes == desc2.Attributes);
}

}
