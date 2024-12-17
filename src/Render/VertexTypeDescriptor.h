#pragma once

#include "Common.h"

namespace render
{

// Descriptor of the vertex attribute
struct VertexAttribute
{
	enum class DataFormat
	{
		Regular,
		Normalized,
		Integral,
	};
	std::string Name;
	u32 ComponentCount;
	BasicType ComponentType;
	DataFormat Format;

	bool operator==(const VertexAttribute &other)
	{
		return (ComponentCount == other.ComponentCount &&
			ComponentType == other.ComponentType && Format == other.Format);
	}
};

// Descriptor of the vertex type
struct VertexTypeDescriptor
{
	std::string Name;
	std::vector<VertexAttribute> Attributes;

	bool operator==(const VertexType &other)
	{
		return Attributes == other.Attributes;
	}
};

// Returns size of the vertex type in bytes
extern size_t sizeOfVertexType(const VertexTypeDescriptor &vtype)
{
	size_t size;
	for (const auto &attr : vtype.Attributes)
		size += attr.ComponentCount * utils::getSizeOfType(attr.ComponentType);

	return size;
}


// Default 3D vertex type (position, normal, color and uv)
enum class DefaultVTypeAttrs
{
	POSITION = 0,
	NORMAL,
	COLOR,
	UV
};

static const VertexTypeDescriptor DefaultVType = {
	"3D_Default",
	{
		{"Position", 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular},
		{"Normal", 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular},
		{"Color", 4, BasicType::UINT8, VertexAttribute::DataFormat::Normalized},
		{"UV", 2, BasicType::FLOAT, VertexAttribute::DataFormat::Regulard},
	}
};

}
