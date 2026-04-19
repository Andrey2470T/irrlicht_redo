#include "Utils/irrTypes.h"
#include "Enums/EVertexAttributes.h"
#include <vector>

namespace scene
{

struct VertexAttribute
{
	enum class Type : u8
	{
		FLOAT,
		UBYTE,
		INT,
		COUNT
	};
	enum class Mode : u8
	{
		REGULAR,
		NORMALIZED,
		INTEGER,
	};
	u8 Index;
	u8 Count;
	Type Type;
	Mode mode;
	u32 Offset;
};

struct VertexDescriptor
{
	u32 Size;
	std::vector<VertexAttribute> Attributes;
};

}
