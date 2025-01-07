#include "VertexTypeDescriptor.h"

namespace render
{

// Returns size of the vertex type in bytes.
size_t sizeOfVertexType(const VertexTypeDescriptor &vtype)
{
    size_t size = 0;
	for (const auto &attr : vtype.Attributes)
		size += attr.ComponentCount * utils::getSizeOfType(attr.ComponentType);

	return size;
}


// Default 3D vertex type (position, color, normal and uv).
const VertexTypeDescriptor DefaultVType{
	"3D_Default",
	{},
	true,
	true,
	2
};

}
