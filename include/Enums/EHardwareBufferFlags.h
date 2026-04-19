// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once


namespace scene
{

enum E_HARDWARE_MAPPING
{
	//! Don't store on the hardware
	EHM_NEVER = 0,

	//! Rarely changed, usually stored completely on the hardware
	EHM_STATIC,

	//! Sometimes changed, driver optimized placement
	EHM_DYNAMIC,

	//! Always changed, cache optimizing on the GPU
	EHM_STREAM,
	EHM_COUNT
};

enum E_BUFFER_FLAG
{
	//! Change the vertex mapping
	EBF_VERTEX = 0x01,
	//! Change the index mapping
	EBF_INDEX = 0x02
};

} // end namespace scene
