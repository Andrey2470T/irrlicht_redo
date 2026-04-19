// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/irrTypes.h"
#include "Enums/EHardwareBufferFlags.h"
#include "Enums/EPrimitiveTypes.h"
#include "Mesh/VertexTypes.h"
#include "Video/HWBuffer.h"
#include <cstddef>

namespace video
{

class VideoDriver;

class VAO
{
public:
	/// @note does not create on GL side
	VAO()
	  : vbo(HWBT_VERTEX), ibo(HWBT_INDEX)
	{}
	/// @note does not free on GL side
	~VAO() = default;

	/// @return ID of the VAO
	u32 getID() const { return ID; }
	/// @return does this refer to an existing VAO?
	bool exists() const { return ID != 0; }

	void bind() const;
	void unbind() const;

	/**
	 * Upload buffer data to GL.
	 *
	 * Changing the size of the buffer is only possible when `offset == 0`.
	 * @param data data pointer
	 * @param size number of bytes
	 * @param offset offset to upload at
	 * @param usage usage pattern passed to GL (only if buffer is new)
	 * @param mustShrink force re-create of buffer if it became smaller
	 * @note modifies GL_ARRAY_BUFFER binding
	 */
	bool upload(
		VideoDriver *driver,
		const void *vertexData, size_t vertexCount,
		const void *indexData, size_t indexCount,
		const scene::VertexDescriptor &vertexDesc=scene::Vertex3D::FORMAT,
		size_t vertexOffset=0, size_t indexOffset=0,
		scene::E_HARDWARE_MAPPING vertexUsage=scene::EHM_STATIC,
		scene::E_HARDWARE_MAPPING indexUsage=scene::EHM_STATIC,
		bool mustShrink=false);

	/**
	 * Free buffer in GL.
	 * @note modifies GL_ARRAY_BUFFER binding
	 */
	void destroy();

private:
	u32 ID = 0;

	HWBuffer vbo;
	HWBuffer ibo;
};

}
