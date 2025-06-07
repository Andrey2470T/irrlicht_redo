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

    bool bound = false;
public:
	Mesh(const VertexTypeDescriptor &descr = DefaultVType);

	Mesh(const void *vertices, u32 verticesCount, const u32 *indices=nullptr,
		 u32 indicesCount=0, const VertexTypeDescriptor &descr = DefaultVType);
	
	~Mesh();

	VertexTypeDescriptor getVertexType() const
	{
		return descriptor;
	}

    void bind()
	{
        if (bound)
            return;
		glBindVertexArray(vaoID);
        bound = true;
	}

    void unbind()
	{
        if (!bound)
            return;
		glBindVertexArray(0);
        bound = false;
	}

    void reallocateVBO(const void *vertices=nullptr, u32 count=0);
    void reallocateIBO(const u32 *indices=nullptr, u32 count=0);
    void uploadVertexData(const void *vertices, u32 count, u32 offset=0);
    void uploadIndexData(const u32 *indices, u32 count, u32 offset=0);

    void draw(PrimitiveType mode = PT_TRIANGLES, u32 count = 0, u32 offset = 0) const;
    void multiDraw(PrimitiveType mode, const s32 *count, const s32 *offset, u32 drawCount);
	
	bool operator==(const Mesh *other)
	{
		return vaoID == other->vaoID;
	}
private:
    void init();
};

}
