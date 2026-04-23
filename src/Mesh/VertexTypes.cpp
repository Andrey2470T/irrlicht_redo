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
		{video::EVA_POSITION, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3D::Pos)},
		{video::EVA_NORMAL, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3D::Normal)},
		{video::EVA_COLOR, 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex3D::Color)},
		{video::EVA_TCOORD0, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3D::TCoords)}
	},
};

const VertexDescriptor Vertex2TCoords::FORMAT = {
	sizeof(Vertex2TCoords),
	{
		{video::EVA_POSITION, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::Pos)},
		{video::EVA_NORMAL, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::Normal)},
		{video::EVA_COLOR, 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex2TCoords::Color)},
		{video::EVA_TCOORD0, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::TCoords)},
		{video::EVA_TCOORD1, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2TCoords::TCoords2)}
	},
};

const VertexDescriptor VertexTangents::FORMAT = {
	sizeof(VertexTangents),
	{
		{video::EVA_POSITION, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Pos)},
		{video::EVA_NORMAL, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Normal)},
		{video::EVA_COLOR, 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&VertexTangents::Color)},
		{video::EVA_TCOORD0, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::TCoords)},
		{video::EVA_TANGENT, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Tangent)},
		{video::EVA_BINORMAL, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&VertexTangents::Binormal)}
	},
};

const VertexDescriptor Vertex2D::FORMAT = {
	sizeof(Vertex2D),
	{
		{video::EVA_POSITION, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2D::Pos)},
		{video::EVA_COLOR, 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex2D::Color)},
		{video::EVA_TCOORD0, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex2D::TCoords)}
	},
};

const VertexDescriptor Vertex3DExt::FORMAT = {
	sizeof(Vertex3DExt),
	{
		{video::EVA_POSITION, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::Pos)},
		{video::EVA_NORMAL, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::Normal)},
		{video::EVA_COLOR, 4, VertexAttribute::Type::UBYTE, VertexAttribute::Mode::NORMALIZED, get_offset(&Vertex3DExt::Color)},
		{video::EVA_TCOORD0, 2, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::TCoords)},
		{video::EVA_AUX, 3, VertexAttribute::Type::FLOAT, VertexAttribute::Mode::REGULAR, get_offset(&Vertex3DExt::Aux)}
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

}
