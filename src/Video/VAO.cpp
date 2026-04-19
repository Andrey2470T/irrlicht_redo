// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "VAO.h"
#include "Common.h"
#include "Video/VideoDriver.h"

#include <cassert>

namespace video
{

void VAO::bind() const
{
	if (!ID)
		return;
	glBindVertexArray(ID);
}

void VAO::unbind() const
{
	if (!ID)
		return;
	glBindVertexArray(0);
}

bool VAO::upload(
	VideoDriver *driver,
	const void *vertexData, size_t vertexCount,
	const void *indexData, size_t indexCount,
	const scene::VertexDescriptor &vertexDesc,
	size_t vertexOffset, size_t indexOffset,
	scene::E_HARDWARE_MAPPING vertexUsage,
	scene::E_HARDWARE_MAPPING indexUsage,
	bool mustShrink)
{
	u32 vertexDataSize = vertexCount * vertexDesc.Size;
	u32 indexDataSize = indexCount * sizeof(u16);
	u32 vertexDataOffset = vertexOffset * vertexDesc.Size;
	u32 indexDataOffset = indexOffset * sizeof(u16);

	bool uploaded = false;

	if (vertexCount > 0) {
		vbo.upload(vertexData, vertexDataSize, vertexDataOffset, vertexUsage, mustShrink);
		uploaded = true;
	}

	if (indexCount > 0) {
		ibo.upload(indexData, indexDataSize, indexDataOffset, indexUsage, mustShrink);
		uploaded = true;
	}

	if (!ID) {
		glGenVertexArrays(1, &ID);

		if (!ID)
			return false;

		bind();

		vbo.bind();

		if (ibo.exists())
			ibo.bind();

		driver->enableAttributeArrays(vertexDesc, 0);

		unbind();
	}

	return uploaded;
}

void VAO::destroy()
{
	if (ID)
		glDeleteVertexArrays(1, &ID);
	ID = 0;

	vbo.destroy();
	ibo.destroy();
}

}
