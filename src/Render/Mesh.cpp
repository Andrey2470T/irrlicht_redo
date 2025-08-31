#include "Mesh.h"
#include "Image/Color.h"
#include "Utils/ByteArray.h"
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

    glBufferData(BufConst(isVBO), count * elemSize, data, glUsage);
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
    
    InfoStream << "Mesh::reallocate() \n";
    if (ibo.ID != 0) {
        InfoStream << "Mesh::reallocate() reallocate ibo\n";
        ibo.reallocate(indices, indexCount);
    }

    unbind();

    TEST_GL_ERROR();
}

void Mesh::uploadVertexData(const void *vertices, u32 count, u32 offset)
{
    bind();

    vbo.upload(vertices, count, offset);

    unbind();

    TEST_GL_ERROR();
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

    TEST_GL_ERROR();
}

void setVertex(ByteArray &ba, v2f pos, img::color8 c, v2f uv, u32 n)
{
    ba.setV2F(pos, n);
    //ba.setFloat(pos.X, n);
    //ba.setFloat(pos.Y, n+1);

    img::setColor8(&ba, c, n+2);
    //ba.setUInt8(c.R(), n+2);
    //ba.setUInt8(c.G(), n+3);
    //ba.setUInt8(c.B(), n+4);
    //ba.setUInt8(c.A(), n+5);

    ba.setV2F(uv, n+6);
    //ba.setFloat(uv.X, n+6);
    //ba.setFloat(uv.Y, n+7);
}

/*void setVertex(ByteArray &ba, v2f pos, f32 c_r, f32 c_g, f32 c_b, f32 c_a, v2f uv, u32 n)
{
    ba.setV2F(pos, n);


    ba.setFloat(c_r, n+2);
    ba.setFloat(c_g, n+3);
    ba.setFloat(c_b, n+4);
    ba.setFloat(c_a, n+5);

    ba.setV2F(uv, n+6);

}*/

void Mesh::createQuad()
{
    /*f32 *vertices = new f32[] {
        -1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,     0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,
        1.0f, -1.0f,   1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f,      0.0f, 0.0f
    };

    u32 *indices = new u32[] {0, 2, 3, 0, 1, 2};*/

    u32 bytes_count = render::sizeOfVertexType(render::VType2D) * 4;

    InfoStream << "bytes_count: " << bytes_count << "\n";
    ByteArray mesh_vertexdata(8 * 4, render::sizeOfVertexType(render::VType2D) * 4);

    setVertex(mesh_vertexdata, v2f(-1.0f, 1.0f), img::white, v2f(0.0f, 1.0f), 0);
    setVertex(mesh_vertexdata, v2f(1.0f, 1.0f), img::white, v2f(1.0f, 1.0f), 8);
    setVertex(mesh_vertexdata, v2f(1.0f, -1.0f), img::white, v2f(1.0f, 0.0f), 16);
    setVertex(mesh_vertexdata, v2f(-1.0f, -1.0f), img::white, v2f(0.0f, 0.0f), 24);

    /*ByteArray mesh_vertexdata(8 * 4, 8 * 4 * sizeof(f32));

    setVertex(mesh_vertexdata, v2f(-1.0f, 1.0f), 1.0f, 1.0f, 1.0f, 1.0f, v2f(0.0f, 1.0f), 0);
    setVertex(mesh_vertexdata, v2f(1.0f, 1.0f), 1.0f, 1.0f, 1.0f, 1.0f, v2f(1.0f, 1.0f), 8);
    setVertex(mesh_vertexdata, v2f(1.0f, -1.0f), 1.0f, 1.0f, 1.0f, 1.0f, v2f(1.0f, 0.0f), 16);
    setVertex(mesh_vertexdata, v2f(-1.0f, -1.0f), 1.0f, 1.0f, 1.0f, 1.0f, v2f(0.0f, 0.0f), 24);*/

    auto color = img::getColor8(&mesh_vertexdata, 2);
    InfoStream << "mesh_vertexdata uv0: " << mesh_vertexdata.getV2F(6) << "\n";
    InfoStream << "mesh_vertexdata color0: " << color.R() << ", " << color.G() << ", " << color.B() << ", " << color.A() << "\n";
    InfoStream << "mesh_vertexdata pos1: " << mesh_vertexdata.getV2F(8) << "\n";

    ByteArray mesh_indexdata(6, sizeof(u32) * 6);
    mesh_indexdata.setUInt32(0, 0);
    mesh_indexdata.setUInt32(2, 1);
    mesh_indexdata.setUInt32(3, 2);
    mesh_indexdata.setUInt32(0, 3);
    mesh_indexdata.setUInt32(1, 4);
    mesh_indexdata.setUInt32(2, 5);

    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vbo.ID);
    glGenBuffers(1, &ibo.ID);

    glBindVertexArray(vaoID);

    /*glBindBuffer(GL_ARRAY_BUFFER, vbo.ID);
    //InfoStream << "createQuad() sizeof vertices: " << (u32)sizeof(vertices) << ", sizeof indices: " << (u32)sizeof(indices) << "\n";
    glBufferData(GL_ARRAY_BUFFER, 32 * sizeof(f32), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(u32), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8*sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8*sizeof(f32), (void*)(2*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(f32), (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);*/

    InfoStream << "createQuad() vertex bytes count: " << mesh_vertexdata.bytesCount() << ", vertex bytes count sizeof: " << (u32)sizeof(mesh_vertexdata.data()) << "\n";
    InfoStream << "createQuad() index bytes count: " << mesh_indexdata.bytesCount() << ", index bytes count sizeof: " << (u32)sizeof(mesh_indexdata.data()) << "\n";
    glBindBuffer(GL_ARRAY_BUFFER, vbo.ID);
    glBufferData(GL_ARRAY_BUFFER, mesh_vertexdata.bytesCount(), mesh_vertexdata.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_indexdata.bytesCount(), mesh_indexdata.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, render::sizeOfVertexType(render::VType2D), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, render::sizeOfVertexType(render::VType2D), (void*)(2*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, render::sizeOfVertexType(render::VType2D), (void*)(2*sizeof(f32) + 4*sizeof(u8)));
    glEnableVertexAttribArray(2);

    /*glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(2*sizeof(f32)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6*sizeof(f32)));
    glEnableVertexAttribArray(2);*/
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

    unbind();

    //glBindVertexArray(vaoID);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

	TEST_GL_ERROR();
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

    unbind();

    TEST_GL_ERROR();
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

    unbind();

    TEST_GL_ERROR();
}

void Mesh::init(bool initIBO)
{
    glGenVertexArrays(1, &vaoID);
    //InfoStream << "Mesh::init() 1\n";
    TEST_GL_ERROR();
	vbo.generate();

    //InfoStream << "Mesh::init() 2\n";
    TEST_GL_ERROR();
    if (initIBO) {
        InfoStream << "Mesh::init() create ibo\n";
        ibo.generate();
        //InfoStream << "Mesh::init() 3\n";
        TEST_GL_ERROR();
    }

    bind();
    glBindBuffer(GL_ARRAY_BUFFER, vbo.ID);

    if (initIBO)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo.ID);

    //InfoStream << "Mesh::init() 4 vertex size: " << (u32)sizeOfVertexType(descriptor) << "\n";
    TEST_GL_ERROR();

    size_t vertexSize = sizeOfVertexType(descriptor);

    std::size_t offset = 0;
    for (int i = 0; i < descriptor.Attributes.size(); i++) {
        auto &attr = descriptor.Attributes[i];

		switch (attr.Format) {
			case VertexAttribute::DataFormat::Regular:
                glVertexAttribPointer(i, (int)attr.ComponentCount, GL_FLOAT, GL_FALSE, vertexSize, (void*)offset);
                break;
			case VertexAttribute::DataFormat::Normalized:
                glVertexAttribPointer(i, (int)attr.ComponentCount, GL_UNSIGNED_BYTE, GL_TRUE, vertexSize, (void*)offset);
                break;
			case VertexAttribute::DataFormat::Integral:
                glVertexAttribIPointer(i, (int)attr.ComponentCount, toGLType[(std::size_t)attr.ComponentType], vertexSize, (void*)offset);
                break;
		};
        //InfoStream << "Mesh::init() 5 component count: " << attr.ComponentCount << ", component type: " << (u8)attr.ComponentType << ", offset: " << (u32)offset << "\n";
        TEST_GL_ERROR();
		glEnableVertexAttribArray(i);
        //InfoStream << "Mesh::init() 6\n";
        TEST_GL_ERROR();

        offset += attr.ComponentCount * getSizeOfType(attr.ComponentType);
	}

    unbind();
    //InfoStream << "Mesh::init() 7\n";

	TEST_GL_ERROR();
}

}
