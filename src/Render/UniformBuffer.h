#pragma once

#include "Common.h"
#include "Utils/ByteArray.h"

namespace render
{

class UniformBuffer
{
	u32 uboID;
	u32 uboBP;

    ByteArray uniformsData;
public:
    UniformBuffer(u32 bindingPoint, const ByteArray &uniforms);

	~UniformBuffer();

	u32 getBindingPoint() const
	{
		return uboBP;
	}

    ByteArray &getUniformsData()
    {
        return uniformsData;
    }

	void bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, uboID);
	}

	void unbind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

    void uploadSubData(u32 offset, u32 size);
	
	bool operator==(const UniformBuffer *other)
	{
		return uboID == other->uboID;
	}
};

}
