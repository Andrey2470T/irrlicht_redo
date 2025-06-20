#include "Mesh.h"
#include "Utils/TypeSize.h"

namespace render
{

inline u32 BufConst(bool isVBO)
{
    return isVBO ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
}

Mesh::BufferObject::~BufferObject()
{
	if (ID != 0) {
	    glDeleteBuffers(1, &ID);
	
	    TEST_GL_ERROR();
	}
}

void Mesh::BufferObject::generate()
{
	glGenBuffers(1, &ID);
}

void Mesh::BufferObject::reallocate(const void *data, u32 count)
{
    glBindBuffer(BufConst(isVBO), ID);
    glBufferData(BufConst(isVBO), count * elemSize, data, (u8)usage);
}

void Mesh::BufferObject::upload(const void *data, u32 count, u32 offset)
{
    glBindBuffer(BufConst(isVBO), ID);
    glBufferSubData(BufConst(isVBO), offset * elemSize, count * elemSize, data);
}

Mesh::Mesh(const VertexTypeDescriptor &descr, bool initIBO, MeshUsage usage)
    : vaoID(0), vbo(true, sizeOfVertexType(descr), usage), ibo(false, sizeof(u32), usage), descriptor(descr)
{
    init(initIBO);

    bind();
    reallocate();
    unbind();
}

Mesh::Mesh(const void *vertices, u32 verticesCount, const u32 *indices,
    u32 indicesCount, const VertexTypeDescriptor &descr, bool initIBO, MeshUsage usage)
    : vaoID(0), vbo(true, sizeOfVertexType(descr), usage), ibo(false, sizeof(u32), usage), descriptor(descr)
{
    init(initIBO);

    bind();
    reallocate(vertices, verticesCount, indices, indicesCount);
    unbind();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vaoID);

	TEST_GL_ERROR();
}

void Mesh::reallocate(const void *vertices, u32 vertexCount, const u32 *indices, u32 indexCount)
{
    bind();

    vbo.reallocate(vertices, vertexCount);
    
    if (ibo.ID != 0)
        ibo.reallocate(indices, indexCount);

    unbind();

    TEST_GL_ERROR();
}

void Mesh::uploadVertexData(const void *vertices, u32 count, u32 offset)
{
    vbo.upload(vertices, count, offset);

    TEST_GL_ERROR();
}

void Mesh::uploadIndexData(const u32 *indices, u32 count, u32 offset)
{
    if (ibo.ID == 0) {
        WarnStream << "Mesh::uploadIndexData() index buffer is not initialized\n";
        return;
    }

    ibo.upload(indices, count, offset);

    TEST_GL_ERROR();
}

void Mesh::draw(PrimitiveType mode, u32 count, u32 offset) const
{
    if (!bound) {
        WarnStream << "Mesh::draw() VAO is not bound\n";
        return;
    }
	GLenum glPType = toGLPrimitiveType[mode];

	switch (mode) {
		case PT_POINTS:
		case PT_POINT_SPRITES:
            glDrawArrays(glPType, offset, count);
			break;
		case PT_LINE_STRIP:
		case PT_LINE_LOOP:
		case PT_LINES:
		case PT_TRIANGLE_STRIP:
		case PT_TRIANGLE_FAN:
		case PT_TRIANGLES:
            glDrawElements(glPType, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(u32)));
			break;
		default:
			break;
	};

	TEST_GL_ERROR();
}

void Mesh::multiDraw(PrimitiveType mode, const s32 *count, const s32 *offset, u32 drawCount)
{
    if (!bound) {
        WarnStream << "Mesh::multiDraw() VAO is not bound\n";
        return;
    }
    GLenum glPType = toGLPrimitiveType[mode];

    switch (mode) {
    case PT_POINTS:
    case PT_POINT_SPRITES:
        glMultiDrawArrays(glPType, offset, count, drawCount);
        break;
    case PT_LINE_STRIP:
    case PT_LINE_LOOP:
    case PT_LINES:
    case PT_TRIANGLE_STRIP:
    case PT_TRIANGLE_FAN:
    case PT_TRIANGLES:
        glMultiDrawElements(glPType, count, GL_UNSIGNED_INT, (const void *const *)(&offset), drawCount);
        break;
    default:
        break;
    };

    TEST_GL_ERROR();
}

void Mesh::init(bool initIBO)
{
    glGenVertexArrays(1, &vaoID);
	vbo.generate();
	
	if (initIBO)
        ibo.generate();

    bind();

    std::size_t offset = 0;
    for (int i = 0; i < descriptor.Attributes.size(); i++) {
        auto &attr = descriptor.Attributes[i];
        size_t vertexSize = sizeOfVertexType(descriptor);
		switch (attr.Format) {
			case VertexAttribute::DataFormat::Regular:
                glVertexAttribPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], GL_FALSE, vertexSize, (void*)offset);
			case VertexAttribute::DataFormat::Normalized:
                glVertexAttribPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], GL_TRUE, vertexSize, (void*)offset);
			case VertexAttribute::DataFormat::Integral:
                glVertexAttribIPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], vertexSize, (void*)offset);
		};
		glEnableVertexAttribArray(i);

        offset += attr.ComponentCount * getSizeOfType(attr.ComponentType);
	}

    unbind();

	TEST_GL_ERROR();
}

}
