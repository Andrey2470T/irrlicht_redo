// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "BasicIncludes.h"

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
	//! Upper left corner
	Vector2D<T> ULC;
	//! Lower right corner
	Vector2D<T> LRC;

	Rect() : ULC(0, 0), LRC(0) {}

	Rect(T x, T y, T x2, T y2)
		: ULC(x, y), LRC(x2, y2) {}

	Rect(const Vector2D<T> &upperLeft, const Vector2D<T> &lowerRight)
		: ULC(upperLeft), LRC(lowerRight) {}

    Rect(const Vector2D<T> &pos, T width, T height)
        : ULC(pos), LRC(pos.X + width, pos.Y + height) {}

	explicit Rect(const Vector2D<T> &size)
		: ULC(0, 0), LRC(size.X, size.Y) {}

	Rect(const Rect &other)
		: ULC(other.ULC), LRC(other.LRC) {}

	Rect<T> &operator=(const Rect<T> &other)
	{
		ULC = other.ULC;
		LRC = other.LRC;

		return *this;
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
		ULC += pos;
		LRC += pos;
		return *this;
	}

	//! move left by given numbers
	Rect<T> operator-(const Vector2D<T> &pos) const
	{
		return *this + (-pos);
	}

	//! move left by given numbers
	Rect<T> &operator-=(const Vector2D<T> &pos)
	{
		*this += (-pos);
		return *this;
	}

	//! equality operator
	constexpr bool operator==(const Rect<T> &other) const
	{
		return (ULC == other.ULC && LRC == other.LRC);
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

	T getWidth() const
	{
		return LRC.X - ULC.X;
	}

	T getHeight() const
	{
		return LRC.Y - ULC.Y;
	}

	v2u getSize() const
	{
		return v2u(getWidth(), getHeight());
	}

	//! Returns size of Rectangle
	T getArea() const
	{
		return getWidth() * getHeight();
	}

	//! Returns if a 2d point is within this Rectangle.
	/** \param pos Position to test if it lies within this rectangle.
	\return True if the position is within the rectangle, false if not. */
	bool isPointInside(const Vector2D<T> &pos) const
	{
		return (ULC.X <= pos.X &&
				ULC.Y <= pos.Y &&
				LRC.X >= pos.X &&
				LRC.Y >= pos.Y);
	}

	//! Return if the other rect is within this rect.
	bool isRectInside(const Rect<T> &other) const
	{
		return (isPointInside(other.ULC) && isPointInside(other.LRC));
	}

	//! Check if the rectangle collides with another rectangle.
	/** \param other Rectangle to test collision with
	\return True if the rectangles collide. */
	bool isRectCollided(const Rect<T> &other) const
	{
		return (LRC.Y > other.ULC.Y &&
				ULC.Y < LRC.Y &&
				LRC.X > other.ULC.X &&
				ULC.X < LRC.X);
	}

	//! Clips this rectangle with another one.
	/** \param other Rectangle to clip with */
	void clipAgainst(const Rect<T> &other)
	{
		if (other.LRC.X < LRC.X)
			LRC.X = other.LRC.X;
		if (other.LRC.Y < LRC.Y)
			LRC.Y = other.LRC.Y;

		if (other.ULC.X > LRC.X)
			LRC.X = other.ULC.X;
		if (other.ULC.Y > LRC.Y)
			LRC.Y = other.ULC.Y;

		if (other.LRC.X < ULC.X)
			ULC.X = other.LRC.X;
		if (other.LRC.Y < ULC.Y)
			ULC.Y = other.LRC.Y;

		if (other.ULC.X > ULC.X)
			ULC.X = other.ULC.X;
		if (other.ULC.Y > ULC.Y)
			ULC.Y = other.ULC.Y;
	}

	//! Moves this rectangle to fit inside another one.
	/** \return True on success, false if not possible */
	bool constrainTo(const Rect<T> &other)
	{
		if (other.getWidth() < getWidth() || other.getHeight() < getHeight())
			return false;

		T diff = other.LRC.X - LRC.X;
		if (diff < 0) {
			LRC.X += diff;
			ULC.X += diff;
		}

		diff = other.LRC.Y - LRC.Y;
		if (diff < 0) {
			LRC.Y += diff;
			ULC.Y += diff;
		}

		diff = ULC.X - other.ULC.X;
		if (diff < 0) {
			ULC.X -= diff;
			LRC.X -= diff;
		}

		diff = ULC.Y - other.ULC.Y;
		if (diff < 0) {
			ULC.Y -= diff;
			LRC.Y -= diff;
		}

		return true;
	}

	//! If the lower right corner of the rect is smaller then the upper left, the points are swapped.
	void repair()
	{
		if (LRC.X < ULC.X) {
			T t = LRC.X;
			LRC.X = ULC.X;
			ULC.X = t;
		}

		if (LRC.Y < ULC.Y) {
			T t = LRC.Y;
			LRC.Y = ULC.Y;
			ULC.Y = t;
		}
	}

	//! Returns if the rect is valid to draw.
	/** It would be invalid if the UpperLeftCorner is lower or more
	right than the LowerRightCorner. */
	bool isValid() const
	{
		return ((LRC.X >= ULC.X) && (LRC.Y >= ULC.Y));
	}

	//! Get the center of the rectangle
	Vector2D<T> getCenter() const
	{
		return Vector2D<T>((ULC + LRC) / 2);
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
		if (x > LRC.X)
			LRC.X = x;
		if (y > LRC.Y)
			LRC.Y = y;

		if (x < ULC.X)
			ULC.X = x;
		if (y < ULC.Y)
			ULC.Y = y;
	}
};

//! Rectangle with float values
typedef Rect<f32> rectf;
//! Rectangle with int values
typedef Rect<s32> recti;
//! Rectangle with unsigned int values
typedef Rect<u32> rectu;

}
