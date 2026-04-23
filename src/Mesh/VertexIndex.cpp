#include "Mesh/VertexIndex.h"

namespace video
{

//! Calculate how many geometric primitives would be drawn
u32 getPrimitiveCount(scene::E_PRIMITIVE_TYPE primitiveType, u32 count)
{
	switch (primitiveType) {
	case scene::EPT_POINTS:
		return count;
	case scene::EPT_LINE_STRIP:
		return count - 1;
	case scene::EPT_LINE_LOOP:
		return count;
	case scene::EPT_LINES:
		return count / 2;
	case scene::EPT_TRIANGLE_STRIP:
		return (count - 2);
	case scene::EPT_TRIANGLE_FAN:
		return (count - 2);
	case scene::EPT_TRIANGLES:
		return count / 3;
	case scene::EPT_POINT_SPRITES:
		return count;
	default:
		return 0;
	}
	return 0;
}

}
