#include "Mesh.h"

namespace render
{

Mesh::Mesh(const VertexTypeDescriptor &descr)
	: vaoID(0), vboID(0), iboID(0), descriptor(descr)
{
	init(descriptor);
}
Mesh::Mesh(const void *vertices, u32 verticesCount, const u32 *indices,
	u32 indicesCount, const VertexTypeDescriptor &descr);
	: vaoID(0), vboID(0), iboID(0), descriptor(descr)
{
	init(descriptor);
	uploadData(vertices, verticesCount, indices, indicesCount);
}

void Mesh::uploadData(const void *vertices, u32 verticesCount, const u32 *indices, u32 indicesCount)
{
	bool vertices_valid = vertices != nullptr && verticesCount > 0;
	bool indices_valid = indices != nullptr && indicesCount > 0;

	if (!vertices_valid && !indices_valid) {
		SDL_LogWarn(LC_VIDEO, "Mesh::uploadSubData() vertices and indices are invalid");
		return;
	}

	glBindVertexArray(vaoID);
	
	if (vertices_valid) {
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, verticesCount * sizeOfVertexType(descriptor), vertices, GL_STATIC_DRAW);
	}

	if (indices_valid) {
		if (iboID == 0)
			glGenBuffers(1, &iboID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesCount * sizeof(u32), indices, GL_STATIC_DRAW);
	}
	
	glBindVertexArray(0);
	
	TEST_GL_ERROR();
}

void Mesh::draw(PrimitiveType mode, u32 indicesCount) const
{
	GLenum glPType = toGLPrimitiveType[mode];

	switch (mode) {
		case PT_POINTS:
		case PT_POINT_SPRITES:
			glDrawArrays(glPType, 0, indicesCount);
			break;
		case PT_LINE_STRIP:
		case PT_LINE_LOOP:
		case PT_LINES:
		case PT_TRIANGLE_STRIP:
		case PT_TRIANGLE_FAN:
		case PT_TRIANGLES:
			glDrawElements(glPType, indicesCount, GL_UNSIGNED_INT, 0);
			break;
		default:
			break;
	};
	
	TEST_GL_ERROR();
}

void Mesh::init(VertexTypeDescriptor descr)
{
	glGenVertexArrays(1, &vaoID);
	glGenBuffers(1, &vboID);
	
	glBindVertexArray(vaoID);
	
	u32 offset = 0;
	for (int i = 0; i < descr.Attributes.size(); i++) {
		auto &attr = descr.Attributes[i];
		size_t vertexSize = sizeOfVertexType(descr);
		switch (attr.Format) {
			case VertexAttribute::DataFormat::Regular:
				glVertexAttribPointer(i, (int)attr.ComponentCount, toGLType[attr.ComponentType], GL_FALSE, vertexSize, (void*)offset);
			case VertexAttribute::DataFormat::Normalized:
				glVertexAttribPointer(i, (int)attr.ComponentCount, toGLType[attr.ComponentType], GL_TRUE, vertexSize, (void*)offset);
			case VertexAttribute::DataFormat::Integral:
				glVertexAttribIPointer(i, (int)attr.ComponentCount, toGLType[attr.ComponentType], vertexSize, (void*)offset);
		};
		glEnableVertexAttribArray(i);
		
		offset += attr.ComponentCount * utils::getSizeOfType(attr.ComponentType);
	}
	
	glBindVertexArray(0);
	
	TEST_GL_ERROR();
}

}
