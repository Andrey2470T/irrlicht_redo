#include "Mesh.h"
#include "Utils/TypeSize.h"

namespace render
{

Mesh::Mesh(const VertexTypeDescriptor &descr)
	: vaoID(0), vboID(0), iboID(0), descriptor(descr)
{
    init();

    bind();
    reallocate(0);
    unbind();
}

Mesh::Mesh(const void *vertices, u32 verticesCount, const u32 *indices,
    u32 indicesCount, const VertexTypeDescriptor &descr)
	: vaoID(0), vboID(0), iboID(0), descriptor(descr)
{
    init();

    bind();
    reallocateVBO(vertices, verticesCount);
    reallocateIBO(indices, indicesCount);
    unbind();
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);

	if (iboID != 0)
		glDeleteBuffers(1, &iboID);

	TEST_GL_ERROR();
}

void Mesh::reallocateVBO(const void *vertices, u32 count)
{
    bind();

    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, count * sizeOfVertexType(descriptor), vertices, GL_STATIC_DRAW);

    if (iboID == 0)
        glGenBuffers(1, &iboID);

     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(u32), indices, GL_STATIC_DRAW);

    unbind();

    TEST_GL_ERROR();
}

void Mesh::reallocateIBO(const u32 *indices, u32 count)
{

}

void Mesh::uploadVertexData(const void *vertices, u32 count, u32 offset)
{
    auto vertexSize = sizeOfVertexType(descriptor);

    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferSubData(GL_ARRAY_BUFFER, offset * vertexSize, count * vertexSize, vertices);
    unbind();

    TEST_GL_ERROR();
}

void Mesh::uploadIndexData(const u32 *indices, u32 count, u32 offset)
{
    if (iboID == 0) {
        WarnStream << "Mesh::uploadIndexData() index buffer is not initialized\n";
        return;
    }

    bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(u32), count * sizeof(u32), indices);
    unbind();

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

void Mesh::init()
{
    glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);

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
