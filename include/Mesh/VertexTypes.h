// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Utils/vector3d.h"
#include "Utils/vector2d.h"
#include "Image/SColor.h"
#include "VertexDescriptor.h"

namespace scene
{

//! Enumeration for all vertex types there are.
enum E_VERTEX_TYPE
{
	//! 3D vertex
	EVT_3D = 0,

	//! 3D vertex with two texture coordinates
	/** Usually used for geometry with lightmaps or other special materials. */
	EVT_2TCOORDS,

	//! 3D vertex with a tangent and binormal vector
	/** Usually used for tangent space normal mapping.
		Usually tangent and binormal get send to shaders as texture coordinate sets 1 and 2.
	*/
	EVT_TANGENTS,

	//! 2D vertex
	EVT_2D,

	//! 3D vertex with additional vec3 atrribute
	EVT_3D_EXT,
};

//! 3D vertex
struct Vertex3D
{
	//! Position
	core::vector3df Pos;

	//! Normal vector
	core::vector3df Normal;

	//! Color
	video::SColor Color{0xffffffff};

	//! Texture coordinates
	core::vector2df TCoords;

	static const VertexDescriptor FORMAT;

	static E_VERTEX_TYPE getType() { return EVT_3D; }
};

//! 3D vertex with two texture coordinates.
/** Usually used for geometry with lightmaps
or other special materials.
*/
struct Vertex2TCoords : public Vertex3D
{
	//! Second set of texture coordinates
	core::vector2df TCoords2;

	static const VertexDescriptor FORMAT;

	static E_VERTEX_TYPE getType() { return EVT_2TCOORDS; }
};

//! 3D vertex with a tangent and binormal vector.
/** Usually used for tangent space normal mapping.
	Usually tangent and binormal get send to shaders as texture coordinate sets 1 and 2.
*/
struct VertexTangents : public Vertex3D
{
	//! Tangent vector along the x-axis of the texture
	core::vector3df Tangent;

	//! Binormal vector (tangent x normal)
	core::vector3df Binormal;

	static const VertexDescriptor FORMAT;

	static E_VERTEX_TYPE getType() { return EVT_TANGENTS; }
};

//! 2D vertex
struct Vertex2D
{
	//! Position
	core::vector2df Pos;

	//! Color
	video::SColor Color{0xffffffff};

	//! Texture coordinates
	core::vector2df TCoords;

	static const VertexDescriptor FORMAT;

	static E_VERTEX_TYPE getType() { return EVT_2D; }
};

//! 3D vertex with the free 'Aux' attribute
struct Vertex3DExt : public Vertex3D
{
	//! Second set of texture coordinates
	core::vector3df Aux;

	static const VertexDescriptor FORMAT;

	static E_VERTEX_TYPE getType() { return EVT_3D_EXT; }
};

const VertexDescriptor &getVertexTypeDescription(E_VERTEX_TYPE type);
u32 getVertexTypeSize(E_VERTEX_TYPE type);

bool operator==(const Vertex3D &a, const Vertex3D &b);
bool operator<(const Vertex3D &a, const Vertex3D &b);

struct CompareVertex3D {
	bool operator()(const Vertex3D& a, const Vertex3D& b) const {
		return a < b;
	}
};

} // end namespace video
