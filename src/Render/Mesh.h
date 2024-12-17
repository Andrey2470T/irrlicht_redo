#pragma once

#include "Common.h"

namespace render
{

class Mesh
{
	u32 vaoID;
	u32 vboID;
	u32 iboID;

	VertexTypeDescriptor descriptor;

	utils::ByteArray verticesCache;
	utils::ByteArray indicesCache;
	
	bool cacheVertexData;
public:
	Mesh(const VertexTypeDescriptor &descr, bool cache = true)
		: descriptor(std::move(descr), cacheVertexData(cache))
	{}
	Mesh(const VertexTypeDescriptor &descr, const void *vertices, u32 verticesCount,
		 const u32 *indices=nullptr, u32 indicesCount=0, bool cache = true);

	VertexTypeDescriptor getVertexType() const
	{
		return descriptor;
	}

	void bind() const
	{
		glBindVertexArray(vaoID);
	}

	void unbind() const
	{
		glBindVertexArray(0);
	}

	void uploadData(const void *vertices, u32 verticesCount, const u32 *indicesCount=nullptr, u32 indicesCount=0)
	{
		uploadSubData(0, vertices, verticesCount, indices, indicesCount)
	}
	void uploadSubData(u32 offset, const void *vertices, u32 verticesCount,
		const u32 *indicesCount=nullptr, u32 indicesCount=0);
	
	void draw(PrimitiveType mode) const;
};

}
