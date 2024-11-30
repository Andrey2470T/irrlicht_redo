// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "BasicIncludes.h"

namespace utils
{

//! 2D line between two points with intersection methods.
template <class T>
class Line2D
{
	//! Start point of the line.
	Vector2D<T> Start;
	//! End point of the line.
	Vector2D<T> End;
public:
	Line2D() : Start(0, 0), End(1, 1) {}

	Line2D(T xa, T ya, T xb, T yb)
		: Start(xa, ya), End(xb, yb) {}

	Line2D(const Vector2D<T> &start, const Vector2D<T> &end)
		: Start(start), End(end) {}

	Line2D(const Line2D &other)
		: Start(other.Start), End(other.End) {}

	Line2D<T> &operator=(const Line2D<T> &other)
	{
		Start = other.Start;
		End = other.End;

		return *this;
	}

	// operators

	Line2D<T> operator+(const Vector2D<T> &point) const { return Line2D<T>(Start + point, End + point); }
	Line2D<T> &operator+=(const Vector2D<T> &point)
	{
		Start += point;
		End += point;
		return *this;
	}

	Line2D<T> operator-(const Vector2D<T> &point) const { return Line2D<T>(Start - point, End - point); }
	Line2D<T> &operator-=(const Vector2D<T> &point)
	{
		Start -= point;
		End -= point;
		return *this;
	}

	bool operator==(const Line2D<T> &other) const
	{
		return (Start == other.Start && End == other.End) || (End == other.Start && Start == other.End);
	}
	bool operator!=(const Line2D<T> &other) const
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
	Vector2D<T> getMiddle() const
	{
		return (Start + End) / (T)2;
	}

	//! Get the vector of the line.
	/** \return The vector of the line. */
	Vector2D<T> getVector() const { return End - Start; }

	//! Get unit vector of the line.
	/** \return Unit vector of this line. */
	Vector2D<T> getUnitVector() const
	{
		T len = (T)(1.0 / getLength());
		return (End - Start) * len;
	}
};

typedef Line2D<f32> line2f;
typedef Line2D<s32> line2i;

}
