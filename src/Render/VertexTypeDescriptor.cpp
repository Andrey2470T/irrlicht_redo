#include "VertexTypeDescriptor.h"

namespace render
{

VertexTypeDescriptor::VertexTypeDescriptor(u8 pos_coords_count, u8 color_cmp_count, bool init_normal, bool init_uv, u8 uv_count)
    : Name(""), PosCoordsCount(pos_coords_count), ColorCmpCount(color_cmp_count),
      InitNormal(init_normal), InitUV(init_uv), UVCount(uv_count)
{
    if (!checkForAttrsCmpsCount())
        return;
    initAttributes();
}

VertexTypeDescriptor::VertexTypeDescriptor(const std::string &name, const std::vector<VertexAttribute> &attributes,
    u8 pos_coords_count, u8 color_cmp_count, bool init_normal, bool init_uv, u8 uv_count)
    : Name(name), PosCoordsCount(pos_coords_count), ColorCmpCount(color_cmp_count),
      InitNormal(init_normal), InitUV(init_uv), UVCount(uv_count)
{
    if (!checkForAttrsCmpsCount())
        return;
    initAttributes();

    for (const auto &attr : attributes)
        appendAttr(attr);
}

VertexTypeDescriptor::VertexTypeDescriptor(const VertexTypeDescriptor &other)
{
    Name = other.Name;
    Attributes = other.Attributes;
    ComponentsCount = other.ComponentsCount;
    PosCoordsCount = other.PosCoordsCount;
    ColorCmpCount = other.ColorCmpCount;
    InitNormal = other.InitNormal;
    InitUV = other.InitUV;
    UVCount = other.UVCount;
}

void VertexTypeDescriptor::appendAttr(const VertexAttribute &attr)
{
    VertexAttribute custom_attr(attr);
    custom_attr.Offset = ComponentsCount;
    ComponentsCount += custom_attr.ComponentCount;
    Attributes.push_back(custom_attr);
}
void VertexTypeDescriptor::initAttributes()
{
    appendAttr({"Position", PosCoordsCount, BasicType::FLOAT, VertexAttribute::DataFormat::Regular});
    appendAttr({"Color", ColorCmpCount, BasicType::UINT8, VertexAttribute::DataFormat::Normalized});

    if (InitNormal)
        appendAttr({"Normal", 3, BasicType::FLOAT, VertexAttribute::DataFormat::Regular});

    if (InitUV)
        appendAttr({"UV", UVCount, BasicType::FLOAT, VertexAttribute::DataFormat::Regular});
}

bool VertexTypeDescriptor::checkForAttrsCmpsCount()
{
    if (PosCoordsCount != 2 && PosCoordsCount != 3) {
        ErrorStream << "VertexTypeDescriptor(): Position coordinates count can be only either 2 or 3\n";
        return false;
    }

    if (ColorCmpCount != 3 && ColorCmpCount != 4) {
        ErrorStream << "VertexTypeDescriptor(): Color components count can be only either 3 or 4\n";
        return false;
    }

    if (UVCount != 2 && UVCount != 3) {
        ErrorStream << "VertexTypeDescriptor(): UV count can be only either 2 or 3\n";
        return false;
    }

    return true;
}

size_t sizeOfBasicType(BasicType type)
{
    switch (type) {
    case BasicType::UINT8:
        return sizeof(u8);
    case BasicType::UINT16:
        return sizeof(u16);
    case BasicType::UINT32:
        return sizeof(u32);
    case BasicType::UINT64:
        return sizeof(u64);
    case BasicType::CHAR:
        return sizeof(s8);
    case BasicType::SHORT:
        return sizeof(s16);
    case BasicType::INT:
        return sizeof(s32);
    case BasicType::LONG_INT:
        return sizeof(s64);
    case BasicType::FLOAT:
        return sizeof(f32);
    case BasicType::DOUBLE:
        return sizeof(f64);
    default:
        return 0;
    };
}

// Returns size of the vertex type in bytes.
size_t sizeOfVertexType(const VertexTypeDescriptor &vtype)
{
    size_t size = 0;
    for (const auto &attr : vtype.Attributes)
        size += attr.ComponentCount * sizeOfBasicType(attr.ComponentType);

    return size;
}


// Default 3D vertex type (position, color, normal and uv).
const VertexTypeDescriptor DefaultVType{
	"Standard3D",
	{},
    3,
    4,
	true,
	true,
	2
};

}
