#pragma once

#include "OpenGLIncludes.h"
#include "Render/toGLEnum.h"
#include "VertexTypeDescriptor.h"
#include <cassert>

namespace render
{

enum class MeshUsage : u8
{
    STATIC = 0,
    DYNAMIC,
    STREAM
};

class Mesh
{
	struct BufferObject
	{
		u32 ID;
		bool isVBO;
		u32 elemSize;
        MeshUsage usage;

        BufferObject(bool _isVBO, u32 size, MeshUsage _usage)
            : isVBO(_isVBO), elemSize(size), usage(_usage)
	    {}
	    ~BufferObject();
	
	    void generate();
        void reallocate(u32 count=0);
	    void upload(const void *data, u32 count, u32 offset=0);
	};
	u32 vaoID;
    BufferObject vbo;
    BufferObject ibo;

	VertexTypeDescriptor descriptor;

    bool bound = false;
public:
    Mesh(const VertexTypeDescriptor &descr, MeshUsage usage);
    Mesh(const VertexTypeDescriptor &descr, bool initIBO=true, MeshUsage usage=MeshUsage::STATIC);
    Mesh(u32 verticesCount, u32 indicesCount=0, const VertexTypeDescriptor &descr = DefaultVType,
        bool initIBO=true, MeshUsage usage=MeshUsage::STATIC);
	
	~Mesh();

	VertexTypeDescriptor getVertexType() const
	{
		return descriptor;
	}

    void bind()
	{
        assert(vaoID != 0);
        if (bound)
            return;
		glBindVertexArray(vaoID);
        bound = true;
	}

    void unbind()
	{
        assert(vaoID != 0);
        if (!bound)
            return;
		glBindVertexArray(0);
        bound = false;
	}

    void reallocate(u32 vertexCount, u32 indexCount=0);
    void uploadVertexData(const void *vertices, u32 count, u32 offset=0);
    void uploadIndexData(const u32 *indices, u32 count, u32 offset=0);

    void draw(PrimitiveType mode = PT_TRIANGLES, u32 count = 0, u32 offset = 0);
    void drawInstanced(PrimitiveType mode = PT_TRIANGLES, u32 count = 0, u32 offset = 0, u32 instanceCount=1);
    void multiDraw(PrimitiveType mode, const s32 *count, const s32 *offset, u32 drawCount);

	bool operator==(const Mesh *other)
	{
		return vaoID == other->vaoID;
	}

    void init(bool initIBO=true, MeshUsage usage=MeshUsage::STATIC);
};

}
