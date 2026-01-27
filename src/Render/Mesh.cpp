#include "Mesh.h"
#include "Image/Color.h"
#include "Utils/ByteArray.h"

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

    TEST_GL_ERROR();
}

void Mesh::BufferObject::reallocate(u32 count)
{
    glBindBuffer(BufConst(isVBO), ID);
    TEST_GL_ERROR();

    u32 glUsage;

    switch (usage) {
    case MeshUsage::STATIC:
        glUsage = GL_STATIC_DRAW;
        break;
    case MeshUsage::DYNAMIC:
        glUsage = GL_DYNAMIC_DRAW;
        break;
    case MeshUsage::STREAM:
        glUsage = GL_STREAM_DRAW;
        break;
    };

    glBufferData(BufConst(isVBO), count * elemSize, nullptr, glUsage);

    TEST_GL_ERROR();
}

void Mesh::BufferObject::upload(const void *data, u32 count, u32 offset)
{
    glBindBuffer(BufConst(isVBO), ID);
    TEST_GL_ERROR();
    glBufferSubData(BufConst(isVBO), offset * elemSize, count * elemSize, data);
    TEST_GL_ERROR();
}

Mesh::Mesh(const VertexTypeDescriptor &descr, bool initIBO, MeshUsage usage)
    : vaoID(0), vbo(true, sizeOfVertexType(descr), usage), ibo(false, sizeof(u32), usage)
{
    init(descr, initIBO, usage);
}

Mesh::Mesh(u32 verticesCount, u32 indicesCount, const VertexTypeDescriptor &descr, bool initIBO, MeshUsage usage)
    : vaoID(0), vbo(true, sizeOfVertexType(descr), usage), ibo(false, sizeof(u32), usage)
{
    init(descr, initIBO, usage);

    bind();
    reallocate(verticesCount, indicesCount);
    unbind();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vaoID);

	TEST_GL_ERROR();
}

void Mesh::reallocate(u32 vertexCount, u32 indexCount)
{
    bind();

    vbo.reallocate(vertexCount);
    
    if (ibo.ID != 0)
        ibo.reallocate(indexCount);

    unbind();
}

void Mesh::uploadVertexData(const void *vertices, u32 count, u32 offset)
{
    bind();

    vbo.upload(vertices, count, offset);

    unbind();
}

void Mesh::uploadIndexData(const u32 *indices, u32 count, u32 offset)
{
    if (ibo.ID == 0) {
        WarnStream << "Mesh::uploadIndexData() index buffer is not initialized\n";
        return;
    }

    bind();

    ibo.upload(indices, count, offset);

    unbind();
}

void Mesh::draw(PrimitiveType mode, u32 count, u32 offset)
{
    bind();

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

    unbind();
}

void Mesh::drawInstanced(PrimitiveType mode, u32 count, u32 offset, u32 instanceCount)
{
    bind();

    GLenum glPType = toGLPrimitiveType[mode];

    switch (mode) {
    case PT_POINTS:
    case PT_POINT_SPRITES:
        glDrawArraysInstanced(glPType, offset, count, instanceCount);
        break;
    case PT_LINE_STRIP:
    case PT_LINE_LOOP:
    case PT_LINES:
    case PT_TRIANGLE_STRIP:
    case PT_TRIANGLE_FAN:
    case PT_TRIANGLES:
        glDrawElementsInstanced(glPType, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(u32)), instanceCount);
        break;
    default:
        break;
    };
    TEST_GL_ERROR();

    unbind();
}

void Mesh::multiDraw(PrimitiveType mode, const s32 *count, const s32 *offset, u32 drawCount)
{
    bind();

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

    unbind();
}

void Mesh::init(const VertexTypeDescriptor &descr, bool initIBO, MeshUsage usage)
{
    glGenVertexArrays(1, &vaoID);
    TEST_GL_ERROR();
    vbo.generate();

    if (initIBO)
        ibo.generate();

    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo.ID);
    TEST_GL_ERROR();

    if (initIBO) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.ID);
        TEST_GL_ERROR();
    }

    size_t vertexSize = sizeOfVertexType(descr);

    std::size_t offset = 0;
    for (int i = 0; i < descr.Attributes.size(); i++) {
        auto &attr = descr.Attributes[i];

		switch (attr.Format) {
			case VertexAttribute::DataFormat::Regular:
                glVertexAttribPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], GL_FALSE, vertexSize, (void*)offset);
                break;
			case VertexAttribute::DataFormat::Normalized:
                glVertexAttribPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], GL_TRUE, vertexSize, (void*)offset);
                break;
			case VertexAttribute::DataFormat::Integral:
                glVertexAttribIPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], vertexSize, (void*)offset);
                break;
		};
        TEST_GL_ERROR();
		glEnableVertexAttribArray(i);
        TEST_GL_ERROR();

        offset += attr.ComponentCount * sizeOfBasicType(attr.ComponentType);
	}

    unbind();
}

}
