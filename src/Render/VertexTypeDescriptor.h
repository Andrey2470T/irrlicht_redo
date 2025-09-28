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
    DataFormat Format = DataFormat::Regular;
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

    u8 PosCoordsCount = 3;
    u8 ColorCmpCount = 4;
    bool InitNormal = false;
    bool InitUV = false;
    u8 UVCount = 0;

	// Constructor with optional passing the normal and uv.
    VertexTypeDescriptor(u8 pos_coords_count=3, u8 color_cmp_count=4, bool init_normal=false, bool init_uv=false, u8 uv_count=0);

	// Constructor with passing the vertex type name, custom attributes and optionally the normal and uv.
	VertexTypeDescriptor(const std::string &name, const std::vector<VertexAttribute> &attributes,
        u8 pos_coords_count=3, u8 color_cmp_count=4, bool init_normal=false, bool init_uv=false, u8 uv_count=0);

    VertexTypeDescriptor(const VertexTypeDescriptor &other);

    bool operator==(const VertexTypeDescriptor &other) const
	{
		return Attributes == other.Attributes;
	}
private:
    void appendAttr(const VertexAttribute &attr);

    void initAttributes();

    bool checkForAttrsCmpsCount();
};

// Returns size of the vertex type in bytes.
extern size_t sizeOfVertexType(const VertexTypeDescriptor &vtype);

// Default 3D vertex type (position, color, normal and uv).
extern const VertexTypeDescriptor DefaultVType;

// Default 2D vertex type (position, color and uv).
//extern const VertexTypeDescriptor VType2D;

}
