#pragma once

#include "Common.h"
#include "utils/ByteArray.h"

namespace render
{

class UniformBuffer
{
	u32 uboID;
	u32 uboBP;

	utils::ByteArray uniformsData;
public:
	UniformBuffer(u32 bindingPoint, const utils::ByteArray &uniforms);

	~UniformBuffer();

	u32 getBindingPoint() const
	{
		return uboBP;
	}

	void bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	}

	void unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	void uploadSubData(u32 offset, const utils::ByteArray &uniforms);
};

}
