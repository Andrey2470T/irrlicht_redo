#include "UniformBuffer.h"

namespace render
{

UniformBuffer::UniformBuffer(u32 bindingPoint, const utils::ByteArray &uniforms)
{
	glGenBuffers(1, &uboID);
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferData(GL_UNIFORM_BUFFER, uniforms.bytesCount(), uniforms.data(), GL_STATIC_DRAW);

	glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, uboID);

	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	uboBP = bindingPoint;
	uniformsData = std::move(uniforms);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &uboID);
}

void UniformBuffer::uploadSubData(u32 offset, const utils::ByteArray &uniforms)
{
	glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, uniforms.bytesCount(), uniforms.data());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

}