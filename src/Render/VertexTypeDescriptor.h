#pragma once

#include "Common.h"

namespace render
{

// Descriptor of the vertex attribute.
struct VertexAttribute
{
	// 'Regular' means the non-normalized attribute.
	enum class DataFormat
	{
		Regular,
		Normalized,
		Integral,
	};
	std::string Name;
	u8 ComponentCount;
	BasicType ComponentType;
	DataFormat Format;
	u8 Offset = 0; // in summary components count from the beginning

	bool operator==(const VertexAttribute other) const
	{
		return (ComponentCount == other.ComponentCount &&
			ComponentType == other.ComponentType && Format == other.Format);
	}
};

// Descriptor of the vertex type.
// Always contains at least 2 attributes (float 3-component position and uint8 color).
// Also optionally create the float 3-component normal and float 2 or 3-component uv.
// These 4 attrubutes are hardcoded for simplicity of the transformations over the meshes
// and passed in the shader always in the same look.
class VertexTypeDescriptor
{
public:
	// Name of the vertex type.
	std::string Name;
	// List of all attributes.
	std::vector<VertexAttribute> Attributes;
	u32 ComponentsCount = 0;

	// Constructor with optional passing the normal and uv.
	VertexTypeDescriptor(bool init_normal=false, bool init_uv=false, u8 uv_count=0)
		: Name("")
	{
		initAttributes(init_normal, init_uv, uv_count);
	}
	// Constructor with passing the vertex type name, custom attributes and optionally the normal and uv.
	VertexTypeDescriptor(const std::string &name, const std::vector<VertexAttribute> &attributes,
		bool init_normal=false, bool init_uv=false, u8 uv_count=0)
		:Name(name)
	{
		initAttributes(init_normal, init_uv, uv_count);

		for (const auto &attr : attributes)
			appendAttr(attr);
	}
    bool operator==(const VertexTypeDescriptor &other) const
	{
		return Attributes == other.Attributes;
	}
private:
	void appendAttr(const VertexAttribute &attr)
	{
		VertexAttribute custom_attr(attr);
		custom_attr.Offset = ComponentsCount;
		ComponentsCount += custom_attr.ComponentCount;
		Attributes.push_back(custom_attr);
	}
	void initAttributes(bool init_normal=false, bool init_uv=false, u8 uv_count=0)
	{
		appendAttr({"Position", 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular});
		appendAttr({"Color", 4, BasicType::UINT8, VertexAttribute::DataFormat::Normalized});

		if (init_normal)
			appendAttr({"Normal", 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular});

		if (init_uv)
            appendAttr({"UV", uv_count, BasicType::FLOAT, VertexAttribute::DataFormat::Regular});
	}
};

// Returns size of the vertex type in bytes.
extern size_t sizeOfVertexType(const VertexTypeDescriptor &vtype);

// Default 3D vertex type (position, color, normal and uv).
extern const VertexTypeDescriptor DefaultVType;

}
