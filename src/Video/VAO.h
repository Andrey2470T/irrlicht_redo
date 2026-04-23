// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/irrTypes.h"
#include "Enums/EHardwareBufferFlags.h"
#include "Enums/EPrimitiveTypes.h"
#include "Mesh/VertexTypes.h"
#include <cstddef>

namespace video
{

class HWBuffer;
class VideoDriver;

class VAO
{
public:
	/// @note does not create on GL side
	VAO() = default;
	/// @note does not free on GL side
	~VAO() = default;

	/// @return ID of the VAO
	u32 getID() const { return ID; }
	/// @return does this refer to an existing VAO?
	bool exists() const { return ID != 0; }

	void bind() const;
	void unbind() const;

	// Update the VAO internal state (the current bound VBO + IBO and enabled attribute arrays)
	void update(
		VideoDriver *driver, const scene::VertexDescriptor &vertexDesc,
		const HWBuffer &newVBO, const HWBuffer &newIBO);

	// Free buffer in GL
	void destroy();

private:
	u32 ID = 0;

	u32 VBO = 0;
	u32 IBO = 0;
};

}
