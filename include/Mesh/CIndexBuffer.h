// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <vector>
#include "IIndexBuffer.h"
#include "Video/HWBuffer.h"

namespace scene
{
//! Template implementation of the IIndexBuffer interface
template <class T>
class CIndexBuffer final : public IIndexBuffer
{
public:
	//! Default constructor for empty buffer
	CIndexBuffer()
		: IBO(video::HWBT_INDEX)
	{}

	~CIndexBuffer()
	{
		IBO.destroy();
	}

	video::E_INDEX_TYPE getType() const override
	{
		static_assert(sizeof(T) == 2 || sizeof(T) == 4, "invalid index type");
		return sizeof(T) == 2 ? video::EIT_16BIT : video::EIT_32BIT;
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

	const video::HWBuffer &getIBO() const override
	{
		return IBO;
	}

	bool reload(video::VideoDriver *driver) override
	{
		if (!Dirty || MappingHint == EHM_NEVER)
			return false;

		Dirty = false;

		return IBO.upload(Data.data(), sizeof(T) * Data.size(), 0, MappingHint);
	}

	//! Indices of this buffer
	std::vector<T> Data;

private:
	bool Dirty = true;
	//! hardware mapping hint
	E_HARDWARE_MAPPING MappingHint = EHM_NEVER;
	mutable video::HWBuffer IBO;
};

//! Standard 16-bit buffer
typedef CIndexBuffer<u16> SIndexBuffer;

} // end namespace scene
