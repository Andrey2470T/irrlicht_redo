// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "BasicIncludes.h"
#include "MathFuncs.h"
#include "Plane3D.h"
#include "Line3D.h"

namespace utils
{

//! Axis aligned bounding box in 3d dimensional space.
/** Has some useful methods used with occlusion culling or clipping.
 */
template <class T>
class AABB
{
public:
	//! The near edge
	Vector3D<T> MinEdge;

	//! The far edge
	Vector3D<T> MaxEdge;

	//! Default Constructor.
    constexpr AABB() : MinEdge(-1, -1, -1), MaxEdge(1, 1, 1) {}
	//! Constructor with min edge and max edge.
    constexpr AABB(const Vector3D<T> &min, const Vector3D<T> &max)
		: MinEdge(min), MaxEdge(max) {}
	//! Constructor with only one point.
    constexpr AABB(const Vector3D<T> &init)
		: MinEdge(init), MaxEdge(init) {}
	//! Constructor with min edge and max edge as single values, not vectors.
    constexpr AABB(T minx, T miny, T minz, T maxx, T maxy, T maxz)
		: MinEdge(minx, miny, minz), MaxEdge(maxx, maxy, maxz) {}

    constexpr AABB(const AABB<T> &other)
		: MinEdge(other.MinEdge), MaxEdge(other.MaxEdge) {}

	AABB<T> &operator=(const AABB<T> &other)
	{
		MinEdge = other.MinEdge;
		MaxEdge = other.MaxEdge;

		return *this;
	}

	// operators
	//! Equality operator
	/** \param other box to compare with.
	\return True if both boxes are equal, else false. */
	inline bool operator==(const AABB<T> &other) const
	{
		return (MinEdge == other.MinEdge && other.MaxEdge == MaxEdge);
	}
	//! Inequality operator
	/** \param other box to compare with.
	\return True if both boxes are different, else false. */
	inline bool operator!=(const AABB<T> &other) const
	{
		return !(*this == other);
	}

	// functions

	//! Resets the bounding box to a one-point box.
	/** \param x X coord of the point.
	\param y Y coord of the point.
	\param z Z coord of the point. */
	void reset(T x, T y, T z)
	{
		MaxEdge = Vector3D<T>(x, y, z);
		MinEdge = MaxEdge;
	}

	//! Resets the bounding box.
	/** \param initValue New box to set this one to. */
	void reset(const AABB<T> &initValue)
	{
		*this = initValue;
	}

	//! Resets the bounding box to a one-point box.
	/** \param initValue New point. */
	void reset(const Vector3D<T> &initValue)
	{
		MaxEdge = initValue;
		MinEdge = initValue;
	}

	//! Adds a point to the bounding box
	/** The box grows bigger, if point was outside of the box.
	\param p: Point to add into the box. */
	void addInternalPoint(const Vector3D<T> &p)
	{
		addInternalPoint(p.X, p.Y, p.Z);
	}

	//! Adds another bounding box
	/** The box grows bigger, if the new box was outside of the box.
	\param b: Other bounding box to add into this box. */
	void addInternalBox(const AABB<T> &b)
	{
		addInternalPoint(b.MaxEdge);
		addInternalPoint(b.MinEdge);
	}

	//! Adds a point to the bounding box
	/** The box grows bigger, if point is outside of the box.
	\param x X coordinate of the point to add to this box.
	\param y Y coordinate of the point to add to this box.
	\param z Z coordinate of the point to add to this box. */
	void addInternalPoint(T x, T y, T z)
	{
		if (x > MaxEdge.X)
			MaxEdge.X = x;
		if (y > MaxEdge.Y)
			MaxEdge.Y = y;
		if (z > MaxEdge.Z)
			MaxEdge.Z = z;

		if (x < MinEdge.X)
			MinEdge.X = x;
		if (y < MinEdge.Y)
			MinEdge.Y = y;
		if (z < MinEdge.Z)
			MinEdge.Z = z;
	}

	//! Get center of the bounding box
	/** \return Center of the bounding box. */
	Vector3D<T> getCenter() const
	{
		return (MinEdge + MaxEdge) / 2;
	}

	//! Get extent of the box (maximal distance of two points in the box)
	/** \return Extent of the bounding box. */
	Vector3D<T> getExtent() const
	{
		return MaxEdge - MinEdge;
	}

	//! Get radius of the bounding sphere
	/** \return Radius of the bounding sphere. */
	T getRadius() const
	{
		const T radius = getExtent().getLength() / 2;
		return radius;
	}

	//! Check if the box is empty.
	/** This means that there is no space between the min and max edge.
	\return True if box is empty, else false. */
	bool isEmpty() const
	{
		return MinEdge.equals(MaxEdge);
	}

	//! Get the volume enclosed by the box in cubed units
	T getVolume() const
	{
		const Vector3D<T> e = getExtent();
		return e.X * e.Y * e.Z;
	}

	//! Get the surface area of the box in squared units
	T getArea() const
	{
		const Vector3D<T> e = getExtent();
		return 2 * (e.X * e.Y + e.X * e.Z + e.Y * e.Z);
	}

	//! Stores all 8 edges of the box into an array
	/** \param edges: Pointer to array of 8 edges. */
	void getEdges(Vector3D<T> *edges) const
	{
		const v3f middle = getCenter();
		const v3f diag = middle - MaxEdge;

		/*
		Edges are stored in this way:
		Hey, am I an ascii artist, or what? :) niko.
			   /3--------/7
			  / |       / |
			 /  |      /  |
			1---------5   |
			|  /2- - -|- -6
			| /       |  /
			|/        | /
			0---------4/
		*/

        edges[0] = Vector3D<T>(middle.X + diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
        edges[1] = Vector3D<T>(middle.X + diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
        edges[2] = Vector3D<T>(middle.X + diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
        edges[3] = Vector3D<T>(middle.X + diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
        edges[4] = Vector3D<T>(middle.X - diag.X, middle.Y + diag.Y, middle.Z + diag.Z);
        edges[5] = Vector3D<T>(middle.X - diag.X, middle.Y - diag.Y, middle.Z + diag.Z);
        edges[6] = Vector3D<T>(middle.X - diag.X, middle.Y + diag.Y, middle.Z - diag.Z);
        edges[7] = Vector3D<T>(middle.X - diag.X, middle.Y - diag.Y, middle.Z - diag.Z);
	}

	//! Repairs the box.
	/** Necessary if for example MinEdge and MaxEdge are swapped. */
	void repair()
	{
		T t;

		if (MinEdge.X > MaxEdge.X) {
			t = MinEdge.X;
			MinEdge.X = MaxEdge.X;
			MaxEdge.X = t;
		}
		if (MinEdge.Y > MaxEdge.Y) {
			t = MinEdge.Y;
			MinEdge.Y = MaxEdge.Y;
			MaxEdge.Y = t;
		}
		if (MinEdge.Z > MaxEdge.Z) {
			t = MinEdge.Z;
			MinEdge.Z = MaxEdge.Z;
			MaxEdge.Z = t;
		}
	}

	// Check if MaxEdge > MinEdge
	bool isValid() const
	{
		if (MinEdge.X > MaxEdge.X)
			return false;
		if (MinEdge.Y > MaxEdge.Y)
			return false;
		if (MinEdge.Z > MaxEdge.Z)
			return false;

		return true;
	}

	//! Calculates a new interpolated bounding box.
	/** d=0 returns other, d=1 returns this, all other values blend between
	the two boxes.
	\param other Other box to interpolate between
	\param d Value between 0.0f and 1.0f.
	\return Interpolated box. */
	AABB<T> interp(const AABB<T> &other, f32 d) const
	{
		return AABB<T>(MinEdge.linInterp(other.MinEdge, d), MaxEdge.linInterp(other.MaxEdge, d));
	}

	//! Determines if a point is within this box.
	/** Border is included (IS part of the box)!
	\param p: Point to check.
	\return True if the point is within the box and false if not */
	bool isPointInside(const Vector3D<T> &p) const
	{
		return (p.X >= MinEdge.X && p.X <= MaxEdge.X &&
				p.Y >= MinEdge.Y && p.Y <= MaxEdge.Y &&
				p.Z >= MinEdge.Z && p.Z <= MaxEdge.Z);
	}

	//! Determines if a point is within this box and not its borders.
	/** Border is excluded (NOT part of the box)!
	\param p: Point to check.
	\return True if the point is within the box and false if not. */
	bool isPointTotalInside(const Vector3D<T> &p) const
	{
		return (p.X > MinEdge.X && p.X < MaxEdge.X &&
				p.Y > MinEdge.Y && p.Y < MaxEdge.Y &&
				p.Z > MinEdge.Z && p.Z < MaxEdge.Z);
	}

	//! Check if this box is completely inside the 'other' box.
	/** \param other: Other box to check against.
	\return True if this box is completely inside the other box,
	otherwise false. */
	bool isFullInside(const AABB<T> &other) const
	{
		return (other.isPointInside(MinEdge) && other.isPointInside(MaxEdge));
	}

	//! Returns the intersection of this box with another, if possible.
	AABB<T> intersect(const AABB<T> &other) const
	{
		AABB<T> out;

		if (!intersectsWithBox(other))
			return out;

		out.MaxEdge.X = std::min(MaxEdge.X, other.MaxEdge.X);
		out.MaxEdge.Y = std::min(MaxEdge.Y, other.MaxEdge.Y);
		out.MaxEdge.Z = std::min(MaxEdge.Z, other.MaxEdge.Z);

		out.MinEdge.X = std::max(MinEdge.X, other.MinEdge.X);
		out.MinEdge.Y = std::max(MinEdge.Y, other.MinEdge.Y);
		out.MinEdge.Z = std::max(MinEdge.Z, other.MinEdge.Z);

		return out;
	}

	//! Determines if the axis-aligned box intersects with another axis-aligned box.
	/** \param other: Other box to check a intersection with.
	\return True if there is an intersection with the other box,
	otherwise false. */
	bool intersectsWithBox(const AABB<T> &other) const
	{
		return (MinEdge.X <= other.MaxEdge.X && MinEdge.Y <= other.MaxEdge.Y && MinEdge.Z <= other.MaxEdge.Z &&
				MaxEdge.X >= other.MinEdge.X && MaxEdge.Y >= other.MinEdge.Y && MaxEdge.Z >= other.MinEdge.Z);
	}

	//! Tests if the box intersects with a line
	/** \param line: Line to test intersection with.
	\return True if there is an intersection , else false. */
	bool intersectsWithLine(const Line3D<T> &line) const
	{
		return intersectsWithLine(line.getMiddle(), line.getVector().normalize(),
				(T)(line.getLength() * 0.5));
	}

	//! Tests if the box intersects with a line
	/** \param linemiddle Center of the line.
	\param linevect Vector of the line.
	\param halflength Half length of the line.
	\return True if there is an intersection, else false. */
	bool intersectsWithLine(const Vector3D<T> &linemiddle,
			const Vector3D<T> &linevect, T halflength) const
	{
		const Vector3D<T> e = getExtent() * (T)0.5;
		const Vector3D<T> t = getCenter() - linemiddle;

		if ((fabs(t.X) > e.X + halflength * fabs(linevect.X)) ||
				(fabs(t.Y) > e.Y + halflength * fabs(linevect.Y)) ||
				(fabs(t.Z) > e.Z + halflength * fabs(linevect.Z)))
			return false;

		T r = e.Y * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.Y);
		if (fabs(t.Y * linevect.Z - t.Z * linevect.Y) > r)
			return false;

		r = e.X * (T)fabs(linevect.Z) + e.Z * (T)fabs(linevect.X);
		if (fabs(t.Z * linevect.X - t.X * linevect.Z) > r)
			return false;

		r = e.X * (T)fabs(linevect.Y) + e.Y * (T)fabs(linevect.X);
		if (fabs(t.X * linevect.Y - t.Y * linevect.X) > r)
			return false;

		return true;
	}
};

typedef AABB<f32> aabbf;
typedef AABB<s32> aabbi;
typedef AABB<s16> aabbs16;

}
