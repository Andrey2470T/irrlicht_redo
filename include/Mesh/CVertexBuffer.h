// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <vector>
#include "IVertexBuffer.h"

// Define to receive warnings when violating the hw mapping hints
//#define VERTEXBUFFER_HINT_DEBUG

#ifdef VERTEXBUFFER_HINT_DEBUG
#include "../src/os.h"
#endif


namespace scene
{
//! Template implementation of the IVertexBuffer interface
template <class T>
class CVertexBuffer final : public IVertexBuffer
{
public:
	//! Default constructor for empty buffer
	CVertexBuffer() {}

	const void *getData() const override
	{
		return Data.data();
	}

	void *getData() override
	{
		return Data.data();
	}

	u32 getCount() const override
	{
		return static_cast<u32>(Data.size());
	}

	scene::E_VERTEX_TYPE getType() const override
	{
		return T::getType();
	}

	const core::vector3df &getPosition(u32 i) const override
	{
		return Data[i].Pos;
	}

	core::vector3df &getPosition(u32 i) override
	{
		return Data[i].Pos;
	}

	const core::vector3df &getNormal(u32 i) const override
	{
		return Data[i].Normal;
	}

	core::vector3df &getNormal(u32 i) override
	{
		return Data[i].Normal;
	}

	const core::vector2df &getTCoords(u32 i) const override
	{
		return Data[i].TCoords;
	}

	core::vector2df &getTCoords(u32 i) override
	{
		return Data[i].TCoords;
	}

	//! Vertices of this buffer
	std::vector<T> Data;
};

//! Standard buffer
typedef CVertexBuffer<scene::Vertex3D> SVertexBuffer;
//! Buffer with two texture coords per vertex, e.g. for lightmaps
typedef CVertexBuffer<scene::Vertex2TCoords> SVertexBufferLightMap;
//! Buffer with vertices having tangents stored, e.g. for normal mapping
typedef CVertexBuffer<scene::VertexTangents> SVertexBufferTangents;

} // end namespace scene
