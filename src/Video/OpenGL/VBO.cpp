// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "VBO.h"

#include <cassert>


namespace video
{

void OpenGLVBO::upload(const void *data, size_t size, size_t offset,
		GLenum usage, bool mustShrink)
{
	bool newBuffer = false;
	assert(!(mustShrink && offset > 0)); // forbidden usage
	if (!m_name) {
		glGenBuffers(1, &m_name);
		if (!m_name)
			return;
		newBuffer = true;
	} else if (size > m_size || mustShrink) {
		newBuffer = size != m_size;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_name);

	if (newBuffer) {
		assert(offset == 0);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);
		m_size = size;
	} else {
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OpenGLVBO::destroy()
{
	if (m_name)
		glDeleteBuffers(1, &m_name);
	m_name = 0;
	m_size = 0;
}

}
