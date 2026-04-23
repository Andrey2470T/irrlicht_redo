// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <vector>
#include "IVertexBuffer.h"
#include "Video/HWBuffer.h"

namespace scene
{
//! Template implementation of the IVertexBuffer interface
template <class T>
class CVertexBuffer final : public IVertexBuffer
{
public:
	//! Default constructor for empty buffer
	CVertexBuffer()
		: VBO(video::HWBT_VERTEX)
	{}

	~CVertexBuffer()
	{
		VBO.destroy();
	}

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

	E_HARDWARE_MAPPING getHardwareMappingHint() const override
	{
		return MappingHint;
	}

	void setHardwareMappingHint(E_HARDWARE_MAPPING NewMappingHint) override
	{
		MappingHint = NewMappingHint;
	}

	void setDirty() override
	{
		Dirty = true;
	}

	bool getDirty() const override
	{
		return Dirty;
	}

	const video::HWBuffer &getVBO() const override
	{
		return VBO;
	}

	bool reload(video::VideoDriver *driver) override
	{
		if (!Dirty || MappingHint == EHM_NEVER)
			return false;

		Dirty = false;

		return VBO.upload(Data.data(), T::FORMAT.Size * Data.size(), 0, MappingHint);
	}

	//! Vertices of this buffer
	std::vector<T> Data;

private:
	bool Dirty = true;
	//! hardware mapping hint
	E_HARDWARE_MAPPING MappingHint = EHM_NEVER;
	mutable video::HWBuffer VBO;
};

//! Standard buffer
typedef CVertexBuffer<scene::Vertex3D> SVertexBuffer;
//! Buffer with two texture coords per vertex, e.g. for lightmaps
typedef CVertexBuffer<scene::Vertex2TCoords> SVertexBufferLightMap;
//! Buffer with vertices having tangents stored, e.g. for normal mapping
typedef CVertexBuffer<scene::VertexTangents> SVertexBufferTangents;

} // end namespace scene
