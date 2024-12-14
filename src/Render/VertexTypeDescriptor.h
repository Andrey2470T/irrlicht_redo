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
	u32 Index;
	u32 ComponentCount;
	BasicType ComponentType;
	DataFormat Format;
	size_t Offset;

	bool operator==(const VertexAttribute &other)
	{
		return (Index == other.Index && ComponentCount == other.ComponentCount &&
			ComponentType == other.ComponentType && Format == other.Format && Offset == other.Offset);
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
		{"Position", DefaultVTypeAttrs::POSITION, 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular, 0},
		{"Normal", DefaultVTypeAttrs::NORMAL, 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular, 3 * getSizeOfType(BasicType::FLOAT)},
		{"Color", DefaultVTypeAttrs::COLOR, 4, BasicType::UINT8, VertexAttribute::DataFormat::Normalized, 2 * 3 * getSizeOfType(BasicType::FLOAT)},
		{"UV", DefaultVTypeAttrs::UV, 2, BasicType::FLOAT, VertexAttribute::DataFormat::Regular, 2 * 3 * getSizeOfType(BasicType::FLOAT) + 4 * getSizeOfType(BasicType::UINT8)},
	}
};

}
