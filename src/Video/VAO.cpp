// Copyright (C) 2024 sfan5
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "VAO.h"
#include "Common.h"
#include "Video/VideoDriver.h"
#include "Video/HWBuffer.h"

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
	// This call is necessary since the VAO doesn't bind own VBO after itself
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VAO::update(
	VideoDriver *driver, const scene::VertexDescriptor &vertexDesc,
	const HWBuffer &newVBO, const HWBuffer &newIBO)
{
	if (newVBO.getID() == VBO && newIBO.getID() == IBO)
		return;

	if (!ID) {
		glGenVertexArrays(1, &ID);

		if (!ID)
			return;
	}

	bind();

	if (newVBO.exists() && newVBO.getID() != VBO) {
		newVBO.bind();
		VBO = newVBO.getID();

		driver->enableAttributeArrays(vertexDesc, 0);
	}
	if (newIBO.exists() && newIBO.getID() != IBO) {
		newIBO.bind();
		IBO = newIBO.getID();
	}

	unbind();
}

void VAO::destroy()
{
	if (ID)
		glDeleteVertexArrays(1, &ID);
	ID = 0;

	VBO = 0;
	IBO = 0;
}

}
