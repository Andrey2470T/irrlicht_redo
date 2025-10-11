#pragma once

#include "Utils/ByteArray.h"

namespace render
{

class UniformBuffer
{
	u32 uboID;
	u32 uboBP;

    ByteArray uniformsData;

    bool bound = false;
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

    void bind();

    void unbind();

    void uploadSubData(u32 offset, u32 size);
	
	bool operator==(const UniformBuffer *other)
	{
		return uboID == other->uboID;
	}
};

}
