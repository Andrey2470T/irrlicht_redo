// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "BasicIncludes.h"
#include "MathFuncs.h"

namespace utils
{

//! Template plane class with some intersection testing methods.
/** It has to be ensured, that the normal is always normalized. The constructors
	and setters of this class will not ensure this automatically. So any normal
	passed in has to be normalized in advance. No change to the normal will be
	made by any of the class methods.
*/
template <class T>
class Plane3D
{
public:
	//! Normal vector of the plane.
	Vector3D<T> Normal;

	//! Distance from origin.
	T D;

	// Constructors
	Plane3D() : Normal(0, 1, 0) { recalculateD(Vector3D<T>(0, 0, 0)); }

	Plane3D(const Vector3D<T> &MPoint, const Vector3D<T> &Normal)
		: Normal(Normal) 
	{ 
		recalculateD(MPoint);
	}

	Plane3D(const Vector3D<T> &point1, const Vector3D<T> &point2, const Vector3D<T> &point3)
	{
		setPlane(point1, point2, point3);
	}

	Plane3D(const Vector3D<T> &normal, const T d) : Normal(normal), D(d) {}
	
	Plane3D(const Plane3D &other) : Normal(other.Normal), D(other.D) {}

	// operators

	bool operator==(const Plane3D<T> &other) const { return (equals(D, other.D) && Normal == other.Normal); }

	bool operator!=(const Plane3D<T> &other) const { return !(*this == other); }

	// functions

	void setPlane(const Vector3D<T> &point, const Vector3D<T> &nvector)
	{
		Normal = nvector;
		recalculateD(point);
	}

	void setPlane(const Vector3D<T> &nvect, T d)
	{
		Normal = nvect;
		D = d;
	}

	void setPlane(const Vector3D<T> &point1, const Vector3D<T> &point2, const Vector3D<T> &point3)
	{
		// creates the plane from 3 memberpoints
		Normal = (point2 - point1).crossProduct(point3 - point1);
		Normal.normalize();

		recalculateD(point1);
	}

	//! Get an intersection with a 3d line.
	/** \param lineVect Vector of the line to intersect with.
	\param linePoint Point of the line to intersect with.
	\param outIntersection Place to store the intersection point, if there is one.
	\return True if there was an intersection, false if there was not.
	*/
	bool getIntersectionWithLine(const Vector3D<T> &linePoint,
			const Vector3D<T> &lineVect,
			Vector3D<T> &outIntersection) const
	{
		T t2 = Normal.dotProduct(lineVect);

		if (t2 == 0)
			return false;

		T t = -(Normal.dotProduct(linePoint) + D) / t2;
		outIntersection = linePoint + (lineVect * t);
		return true;
	}

	//! Get percentage of line between two points where an intersection with this plane happens.
	/** Only useful if known that there is an intersection.
	\param linePoint1 Point1 of the line to intersect with.
	\param linePoint2 Point2 of the line to intersect with.
	\return Where on a line between two points an intersection with this plane happened.
	For example, 0.5 is returned if the intersection happened exactly in the middle of the two points.
	*/
	f32 getKnownIntersectionWithLine(const Vector3D<T> &linePoint1,
			const Vector3D<T> &linePoint2) const
	{
		Vector3D<T> vect = linePoint2 - linePoint1;
		T t2 = (f32)Normal.dotProduct(vect);
		return (f32) - ((Normal.dotProduct(linePoint1) + D) / t2);
	}

	//! Recalculates the distance from origin by applying a new member point to the plane.
	void recalculateD(const Vector3D<T> &MPoint)
	{
		D = -MPoint.dotProduct(Normal);
	}

	//! Gets a member point of the plane.
	Vector3D<T> getMemberPoint() const
	{
		return Normal * -D;
	}

	//! Intersects this plane with another.
	/** \param other Other plane to intersect with.
	\param outLinePoint Base point of intersection line.
	\param outLineVect Vector of intersection.
	\return True if there is a intersection, false if not. */
	bool getIntersectionWithPlane(const Plane3D<T> &other,
			Vector3D<T> &outLinePoint,
			Vector3D<T> &outLineVect) const
	{
		const T fn00 = Normal.getLength();
		const T fn01 = Normal.dotProduct(other.Normal);
		const T fn11 = other.Normal.getLength();
		const f64 det = fn00 * fn11 - fn01 * fn01;

		if (fabs(det) < ROUNDING_ERROR_f64)
			return false;

		const f64 invdet = 1.0 / det;
		const f64 fc0 = (fn11 * -D + fn01 * other.D) * invdet;
		const f64 fc1 = (fn00 * -other.D + fn01 * D) * invdet;

		outLineVect = Normal.crossProduct(other.Normal);
		outLinePoint = Normal * (T)fc0 + other.Normal * (T)fc1;
		return true;
	}

	//! Get the intersection point with two other planes if there is one.
	bool getIntersectionWithPlanes(const Plane3D<T> &o1,
			const Plane3D<T> &o2, Vector3D<T> &outPoint) const
	{
		Vector3D<T> linePoint, lineVect;
		if (getIntersectionWithPlane(o1, linePoint, lineVect))
			return o2.getIntersectionWithLine(linePoint, lineVect, outPoint);

		return false;
	}

	//! Get the distance to a point.
	/** Note that this only works if the normal is normalized. */
	T getDistanceTo(const Vector3D<T> &point) const
	{
		return point.dotProduct(Normal) + D;
	}
};

//! Typedef for a f32 3d plane.
typedef Plane3D<f32> plane3f;

//! Typedef for an integer 3d plane.
typedef Plane3D<s32> plane3i;

}
