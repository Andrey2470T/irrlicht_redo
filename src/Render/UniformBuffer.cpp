#include "UniformBuffer.h"
#include "Render/Common.h"
#include "OpenGLIncludes.h"

namespace render
{

UniformBuffer::UniformBuffer(u32 bindingPoint, const ByteArray &uniforms)
{
	glGenBuffers(1, &uboID);
    TEST_GL_ERROR();
    bind();
	glBufferData(GL_UNIFORM_BUFFER, uniforms.bytesCount(), uniforms.data(), GL_STATIC_DRAW);
    TEST_GL_ERROR();

	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboID);
    TEST_GL_ERROR();

    unbind();

	uboBP = bindingPoint;
    uniformsData = uniforms;
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &uboID);
    TEST_GL_ERROR();
}

void UniformBuffer::bind()
{
    if (bound)
        return;
    glBindBuffer(GL_UNIFORM_BUFFER, uboID);
    TEST_GL_ERROR();
    bound = true;
}

void UniformBuffer::unbind()
{
    if (!bound)
        return;
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    TEST_GL_ERROR();
    bound = false;
}

void UniformBuffer::uploadSubData(u32 offset, u32 size)
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, uniformsData.data());
    TEST_GL_ERROR();
    unbind();
}

}
