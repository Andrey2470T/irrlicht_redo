#pragma once

#include "VertexTypeDescriptor.h"

namespace render
{

class Mesh
{
	u32 vaoID;
	u32 vboID;
	u32 iboID;

	VertexTypeDescriptor descriptor;
public:
	Mesh(const VertexTypeDescriptor &descr = DefaultVType);

	Mesh(const void *vertices, u32 verticesCount, const u32 *indices=nullptr,
		 u32 indicesCount=0, const VertexTypeDescriptor &descr = DefaultVType);
	
	~Mesh();

	VertexTypeDescriptor getVertexType() const
	{
		return descriptor;
	}

	void bind() const
	{
		glBindVertexArray(vaoID);
	}

	void unbind() const
	{
		glBindVertexArray(0);
	}

	void uploadData(const void *vertices, u32 verticesCount, const u32 *indices=nullptr, u32 indicesCount=0);

	void draw(PrimitiveType mode = PT_TRIANGLES, u32 indicesCount = 0) const;
	
	bool operator==(const Mesh *other)
	{
		return vaoID == other->vaoID;
	}
	
	bool operator!=(const Mesh *other)
	{
		return vaoID != other->vaoID;
	}
private:
	void init(VertexTypeDescriptor descr);
};

}
