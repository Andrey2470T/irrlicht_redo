// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Typedefs.h"
#include "Vector2D.h"

namespace utils
{

//! 2D line between two points with intersection methods.
template <class T>
class Line2D
{
	//! Start point of the line.
	Vector2D<T> start;
	//! End point of the line.
	Vector2D<T> end;
public:
	Line2D() : start(0, 0), end(1, 1) {}

	Line2D(T xa, T ya, T xb, T yb) : start(xa, ya), end(xb, yb) {}

	Line2D(const Vector2D<T> &start, const Vector2D<T> &end) :
			start(start), end(end) {}

	// operators

	Line2D<T> operator+(const Vector2D<T> &point) const { return Line2D<T>(start + point, end + point); }
	Line2D<T> &operator+=(const Vector2D<T> &point)
	{
		start += point;
		end += point;
		return *this;
	}

	Line2D<T> operator-(const Vector2D<T> &point) const { return Line2D<T>(start - point, end - point); }
	Line2D<T> &operator-=(const Vector2D<T> &point)
	{
		start -= point;
		end -= point;
		return *this;
	}

	constexpr bool operator==(const Line2D<T> &other) const
	{
		return (start == other.start && end == other.end) || (end == other.start && start == other.end);
	}
	constexpr bool operator!=(const Line2D<T> &other) const
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
	Vector2D<T> getMiddle() const
	{
		return (start + end) / (T)2;
	}

	//! Get the vector of the line.
	/** \return The vector of the line. */
	Vector2D<T> getVector() const { return end - start; }

	/*! Check if 2 lines/segments are parallel or nearly parallel.*/
	bool nearlyParallel(const Line2D<T> &line, const T factor = relativeErrorFactor<T>()) const
	{
		const Vector2D<T> a = getVector();
		const Vector2D<T> b = line.getVector();

		return a.nearlyParallel(b, factor);
	}

	//! Get unit vector of the line.
	/** \return Unit vector of this line. */
	Vector2D<T> getUnitVector() const
	{
		T len = (T)(1.0 / getLength());
		return (end - start) * len;;
	}

	//! Tells us if the given point lies to the left, right, or on the line.
	/** \return 0 if the point is on the line
	<0 if to the left, or >0 if to the right. */
	T getPointOrientation(const Vector2D<T> &point) const
	{
		return ((end.X - start.X) * (point.Y - start.Y) -
				(point.X - start.X) * (end.Y - start.Y));
	}

	//! Check if the given point is a member of the line
	/** \return True if point is between start and end, else false. */
	bool isPointOnLine(const Vector2D<T> &point) const
	{
		T d = getPointOrientation(point);
		return (d == 0 && point.isBetweenPoints(start, end));
	}

	//! Check if the given point is between start and end of the line.
	/** Assumes that the point is already somewhere on the line. */
	bool isPointBetweenStartAndEnd(const Vector2D<T> &point) const
	{
		return point.isBetweenPoints(start, end);
	}
};


typedef Line2D<f32> line2f;
typedef Line2D<s32> line2i;

}
