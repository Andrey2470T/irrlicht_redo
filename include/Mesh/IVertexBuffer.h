// Copyright (C) 2008-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/IReferenceCounted.h"
#include "Enums/EHardwareBufferFlags.h"
#include "Mesh/VertexTypes.h"


namespace scene
{

class IVertexBuffer : public virtual IReferenceCounted
{
public:
	//! Get type of vertex data which is stored in this meshbuffer.
	/** \return Vertex type of this buffer. */
	virtual scene::E_VERTEX_TYPE getType() const = 0;

	//! Get access to vertex data. The data is an array of vertices.
	/** Which vertex type is used can be determined by getVertexType().
	\return Pointer to array of vertices. */
	virtual const void *getData() const = 0;

	//! Get access to vertex data. The data is an array of vertices.
	/** Which vertex type is used can be determined by getVertexType().
	\return Pointer to array of vertices. */
	virtual void *getData() = 0;

	//! Get amount of vertices in meshbuffer.
	/** \return Number of vertices in this buffer. */
	virtual u32 getCount() const = 0;

	//! returns position of vertex i
	virtual const core::vector3df &getPosition(u32 i) const = 0;

	//! returns position of vertex i
	virtual core::vector3df &getPosition(u32 i) = 0;

	//! returns normal of vertex i
	virtual const core::vector3df &getNormal(u32 i) const = 0;

	//! returns normal of vertex i
	virtual core::vector3df &getNormal(u32 i) = 0;

	//! returns texture coord of vertex i
	virtual const core::vector2df &getTCoords(u32 i) const = 0;

	//! returns texture coord of vertex i
	virtual core::vector2df &getTCoords(u32 i) = 0;
};

} // end namespace scene
