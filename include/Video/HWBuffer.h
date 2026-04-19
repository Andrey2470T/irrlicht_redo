// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/irrTypes.h"
#include "Enums/EHardwareBufferFlags.h"
#include <cstddef>


namespace video
{

enum HWBufferType : u8
{
	HWBT_VERTEX = 0,
	HWBT_INDEX,
	HWBT_UNIFORM,
	HWBT_COUNT
};

class HWBuffer
{
public:
	/// @note does not create on GL side
	HWBuffer(HWBufferType _type, u32 _bindPoint=0)
		: bindPoint(_bindPoint), type(_type)
	{}
	/// @note does not free on GL side
	~HWBuffer() = default;

	/// @return ID of this buffer in GL
	u32 getID() const { return ID; }
	/// @return does this refer to an existing GL buffer?
	bool exists() const { return ID != 0; }

	/// @return size of this buffer in bytes
	size_t getSize() const { return curSize; }

	void bind() const;
	void unbind() const;

	u32 getBindingPoint() const { return bindPoint; }

	void bindToPoint();

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
	void upload(const void *data, size_t size, size_t offset=0,
		scene::E_HARDWARE_MAPPING usage=scene::EHM_STATIC, bool mustShrink=false);

	/**
	 * Free buffer in GL.
	 * @note modifies GL_ARRAY_BUFFER binding
	 */
	void destroy();

private:
	u32 ID = 0;
	u32 bindPoint;
	HWBufferType type;
	size_t curSize = 0;
};

}
