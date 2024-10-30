// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Typedefs.h"
#include "Vector2D.h"

namespace utils
{

//! Rectangle template.
/** Mostly used by 2D GUI elements and for 2D drawing methods.
It has 2 positions instead of position and dimension and a fast
method for collision detection with other rectangles and points.

Coordinates are (0,0) for top-left corner, and increasing to the right
and to the bottom.
*/
template <class T>
class Rect
{
public:
	Vector2D<T> Origin;

	T Width;
	T Height;

	Rect() : Origin(0, 0), Width(0), Height(0) {}

	Rect(T x, T y, T x2, T y2) :
			Origin(x, y), Width(x2 - x), Height(y2 - y) {}

	Rect(const Vector2D<T> &upperLeft, const Vector2D<T> &lowerRight) :
			Origin(upperLeft), Width(lowerRight.X - upperLeft.X),
			Height(lowerRight.Y - upperLeft.Y) {}

	Rect(const Vector2D<T> &pos, const Vector2D<T> &size) :
			Origin(pos), Width(size.X), Height(size.Y) {}

	explicit Rect(const Vector2D &size) :
			Origin(0, 0), Width(size.X), Height(size.Y)
	{
	}

	//! move right by given numbers
	Rect<T> operator+(const Vector2D<T> &pos) const
	{
		Rect<T> rect(*this);
		return rect += pos;
	}

	//! move right by given numbers
	Rect<T> &operator+=(const Vector2D<T> &pos)
	{
		Origin += pos;
		return *this;
	}

	//! move left by given numbers
	Rect<T> operator-(const Vector2D<T> &pos) const
	{
		Rect<T> rect(*this);
		return rect -= pos;
	}

	//! move left by given numbers
	Rect<T> &operator-=(const Vector2D<T> &pos)
	{
		Origin -= pos;
		return *this;
	}

	//! equality operator
	constexpr bool operator==(const Rect<T> &other) const
	{
		return (Origin == other.Origin &&
				Width == other.Width &&
				Height == other.Height);
	}

	//! inequality operator
	constexpr bool operator!=(const Rect<T> &other) const
	{
		return !(*this == other);
	}

	//! compares size of Rectangles
	bool operator<(const Rect<T> &other) const
	{
		return getArea() < other.getArea();
	}

	//! Returns size of Rectangle
	T getArea() const
	{
		return Width * Height;
	}

	Vector2D<T> getEnd() const
	{
		return Origin + Vector2D(Width, Height);
	}

	//! Returns if a 2d point is within this Rectangle.
	/** \param pos Position to test if it lies within this rectangle.
	\return True if the position is within the rectangle, false if not. */
	bool isPointInside(const Vector2D<T> &pos) const
	{
		Vector2D<T> end = getEnd();
		return (Origin.X <= pos.X &&
				Origin.Y <= pos.Y &&
				end.X >= pos.X &&
				end.Y >= pos.Y);
	}

	//! Check if the rectangle collides with another rectangle.
	/** \param other Rectangle to test collision with
	\return True if the rectangles collide. */
	bool isRectCollided(const Rect<T> &other) const
	{
		Vector2D<T> end = getEnd();
		Vector2D<T> other_end = other.getEnd();
		return (end.Y > other.Origin.Y &&
				Origin.Y < other_end.Y &&
				end.X > other.Origin.X &&
				Origin.X < other_end.X);
	}

	//! Clips this rectangle with another one.
	/** \param other Rectangle to clip with */
	void clipAgainst(const Rect<T> &other)
	{
		Vector2D<T> end = getEnd();
		Vector2D<T> other_end = other.getEnd();

		if (other_end.X < end.X)
			end.X = other_end.X;
		if (other_end.Y < end.Y)
			end.Y = other_end.Y;

		if (other.Origin.X > end.X)
			end.X = other.Origin.X;
		if (other.Origin.Y > end.Y)
			end.Y = other.Origin.Y;

		if (other_end.X < Origin.X)
			Origin.X = other_end.X;
		if (other_end.Y < Origin.Y)
			Origin.Y = other_end.Y;

		if (other.Origin.X > Origin.X)
			Origin.X = other.Origin.X;
		if (other.Origin.Y > Origin.Y)
			Origin.Y = other.Origin.Y;

		Width = end.X - Origin.X;
		Height = end.Y - Origin.Y;
	}

	//! Moves this rectangle to fit inside another one.
	/** \return True on success, false if not possible */
	bool constrainTo(const Rect<T> &other)
	{
		if (other.Width < Width || other.Height < Height)
			return false;

		Vector2D<T> end = getEnd();
		Vector2D<T> other_end = other.getEnd();

		T diff = other_end.X - end.X;
		if (diff < 0) {
			end.X += diff;
			Origin.X += diff;
		}

		diff = other_end.Y - end.Y;
		if (diff < 0) {
			end.Y += diff;
			Origin.Y += diff;
		}

		diff = Origin.X - other.Origin.X;
		if (diff < 0) {
			Origin.X -= diff;
			end.X -= diff;
		}

		diff = Origin.Y - other.Origin.Y;
		if (diff < 0) {
			Origin.Y -= diff;
			end.Y -= diff;
		}

		Width = end.X - Origin.X;
		Height = end.Y - Origin.Y;

		return true;
	}

	//! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (Width < 0) {
			Origin.X += Width;
			Width *= -1;
		}

		if (Height < 0) {
			Origin.Y += Height;
			Height *= -1;
		}
	}

	//! Returns if the rect is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
		return (Width >= 0 && Height >= 0);
	}

	//! Get the center of the rectangle
	Vector2D<T> getCenter() const
	{
		return Vector2D<T>((Origin + getEnd()) / 2);
	}

	//! Adds a point to the rectangle
	/** Causes the rectangle to grow bigger if point is outside of
	the box
	\param p Point to add to the box. */
	void addInternalPoint(const Vector2D<T> &p)
	{
		addInternalPoint(p.X, p.Y);
	}

	//! Adds a point to the bounding rectangle
	/** Causes the rectangle to grow bigger if point is outside of
	the box
	\param x X-Coordinate of the point to add to this box.
	\param y Y-Coordinate of the point to add to this box. */
	void addInternalPoint(T x, T y)
	{
		Vector2D<T> end = getEnd();
		if (x > end.X)
			Width = x - Origin.X;
		if (y > end.Y)
			Height = y - Origin.Y;

		if (x < Origin.X) {
			Width += (Origin.X - x);
			Origin.X = x;
		}
		if (y < Origin.Y) {
			Height += (Origin.Y - y)
			Origin.Y = y;
		}
	}
};

//! Rectangle with float values
typedef Rect<f32> rectf;
//! Rectangle with int values
typedef Rect<s32> recti;

} // end namespace core
} // end namespace irr
