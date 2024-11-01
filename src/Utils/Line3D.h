// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Typedefs.h"
#include "Vector3D.h"

namespace utils
{

//! 3D line between two points with intersection methods.
template <class T>
class Line3D
{
	//! Start point of the line.
	Vector3D<T> start;
	//! End point of the line.
	Vector3D<T> end;
public:
	Line3D() : start(0, 0, 0), end(1, 1, 1) {}

	Line3D(T xa, T ya, T za, T xb, T yb, T zb)
		: start(xa, ya, za), end(xb, yb, zb) {}

	Line3D(const Vector3D<T> &start, const Vector3D<T> &end) :
			start(start), end(end) {}

	// operators

	Line3D<T> operator+(const Vector3D<T> &point) const { return Line3D<T>(start + point, end + point); }
	Line3D<T> &operator+=(const Vector3D<T> &point)
	{
		start += point;
		end += point;
		return *this;
	}

	Line3D<T> operator-(const Vector3D<T> &point) const { return Line3D<T>(start - point, end - point); }
	Line3D<T> &operator-=(const Vector3D<T> &point)
	{
		start -= point;
		end -= point;
		return *this;
	}

	constexpr bool operator==(const Line3D<T> &other) const
	{
		return (start == other.start && end == other.end) || (end == other.start && start == other.end);
	}
	constexpr bool operator!=(const Line3D<T> &other) const
	{
		return !(start == other.start && end == other.end) || (end == other.start && start == other.end);
	}

	//! Get length of line
	/** \return Length of the line. */
	T getLength() const { return start.getDistanceFrom(end); }

	//! Get squared length of the line
	/** \return Squared length of line. */
	T getLengthSQ() const { return start.getDistanceFromSQ(end); }

	//! Get middle of the line
	/** \return center of the line. */
	Vector3D<T> getMiddle() const
	{
		return (start + end) / (T)2;
	}

	//! Get the vector of the line.
	/** \return The vector of the line. */
	Vector3D<T> getVector() const { return end - start; }

	//! Get unit vector of the line.
	/** \return Unit vector of this line. */
	Vector2D<T> getUnitVector() const
	{
		T len = (T)(1.0 / getLength());
		return (end - start) * len;
	}
};


typedef Line3D<f32> line3f;
typedef Line3D<s32> line3i;

}
