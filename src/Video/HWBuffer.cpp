// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Video/HWBuffer.h"
#include "Common.h"

#include <cassert>
#include <array>


namespace video
{

std::array<GLenum, HWBT_COUNT> toGLTarget = {
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_UNIFORM_BUFFER
};

std::array<GLenum, scene::EHM_COUNT> toGLUsage = {
	0,
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
	GL_STREAM_DRAW
};

void HWBuffer::bind() const
{
	glBindBuffer(toGLTarget[type], ID);
}

void HWBuffer::unbind() const
{
	glBindBuffer(toGLTarget[type], 0);
}

void HWBuffer::bindToPoint()
{
	assert(type == HWBT_UNIFORM);

	glBindBufferBase(GL_UNIFORM_BUFFER, bindPoint, ID);
}

void HWBuffer::upload(const void *data, size_t size, size_t offset,
		scene::E_HARDWARE_MAPPING usage, bool mustShrink)
{
	bool reallocate = false;
	assert(!(mustShrink && offset > 0)); // forbidden usage
	if (!ID) {
		glGenBuffers(1, &ID);

		if (!ID)
			return;

		if (type == HWBT_UNIFORM)
			bindToPoint();

		reallocate = true;
	} else if (size > curSize || mustShrink) {
		reallocate = size != curSize;
	}

	bind();

	if (reallocate) {
		assert(offset == 0);
		glBufferData(toGLTarget[type], size, data, toGLUsage[usage]);
		curSize = size;
	} else {
		glBufferSubData(toGLTarget[type], offset, size, data);
	}

	unbind();
}

void HWBuffer::destroy()
{
	if (ID)
		glDeleteBuffers(1, &ID);

	ID = 0;
	curSize = 0;
}

}
