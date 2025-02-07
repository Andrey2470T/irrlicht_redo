// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "BasicIncludes.h"

namespace utils
{

//! 3D line between two points with intersection methods.
template <class T>
class Line3D
{
public:
	//! Start point of the line.
	Vector3D<T> Start;
	//! End point of the line.
	Vector3D<T> End;

	constexpr Line3D() : Start(0, 0, 0), End(1, 1, 1) {}

	constexpr Line3D(T xa, T ya, T za, T xb, T yb, T zb)
		: Start(xa, ya, za), End(xb, yb, zb) {}

	constexpr Line3D(const Vector3D<T> &start, const Vector3D<T> &end)
		: Start(start), End(end) {}

	constexpr Line3D(const Line3D &other)
		: Start(other.Start), End(other.End) {}

	Line3D<T> &operator=(const Line3D<T> &other)
	{
		Start = other.Start;
		End = other.End;

		return *this;
	}

	// operators

	Line3D<T> operator+(const Vector3D<T> &point) const { return Line3D<T>(Start + point, End + point); }
	Line3D<T> &operator+=(const Vector3D<T> &point)
	{
		Start += point;
		End += point;
		return *this;
	}

	Line3D<T> operator-(const Vector3D<T> &point) const { return Line3D<T>(Start - point, End - point); }
	Line3D<T> &operator-=(const Vector3D<T> &point)
	{
		Start -= point;
		End -= point;
		return *this;
	}

	bool operator==(const Line3D<T> &other) const
	{
		return (Start == other.Start && End == other.End) || (End == other.Start && Start == other.End);
	}
	bool operator!=(const Line3D<T> &other) const
	{
		return !(*this == other);
	}

	//! Get length of line
	/** \return Length of the line. */
	T getLength() const { return Start.getDistanceFrom(End); }

	//! Get squared length of the line
	/** \return Squared length of line. */
	T getLengthSQ() const { return Start.getDistanceFromSQ(End); }

	//! Get middle of the line
	/** \return center of the line. */
	Vector3D<T> getMiddle() const
	{
		return (Start + End) / (T)2;
	}

	//! Get the vector of the line.
	/** \return The vector of the line. */
	Vector3D<T> getVector() const { return End - Start; }

	//! Get unit vector of the line.
	/** \return Unit vector of this line. */
	Vector2D<T> getUnitVector() const
	{
		T len = (T)(1.0 / getLength());
		return (End - Start) * len;
	}
};

typedef Line3D<f32> line3f;
typedef Line3D<s32> line3i;

}
