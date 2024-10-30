#pragma once

#include "MathFuncs.h"

#include <functional>
#include <array>

namespace utils
{

//! 2d vector template class
template <class T>
class Vector2D
{
public:
	//! X coordinate of vector.
	T X;

	//! Y coordinate of vector.
	T Y;

	Vector2D() : X(0), Y(0) {}

	Vector2D(T x, T y) : X(x), Y(y) {}

	explicit Vector2D(T n) : X(n), Y(n) {}

	Vector2D(const Vector2D<T> &other) : X(other.X), Y(other.Y) {}

	Vector2D<T> &operator=(const Vector2D<T> &other)
	{
		X = other.X;
		Y = other.Y;
		return *this;
	}

	// Operators

	//! Negation
	Vector2D<T> operator-() const { return Vector2D<T>(-X, -Y); }

	//! Add
	Vector2D<T> operator+(const Vector2D<T> &other) const
	{
		return Vector2D<T>(X + other.X, Y + other.Y);
	}

	Vector2D<T> &operator+=(const Vector2D<T> &other)
	{
		X += other.X;
		Y += other.Y;
		return *this;
	}

	Vector2D<T> operator+(const T v) const {
		return Vector2D<T>(X + v, Y + v);
	}

	Vector2D<T> &operator+=(const T v)
	{
		X += v;
		Y += v;
		return *this;
	}

	//! Subtraction
	Vector2D<T> operator-(const Vector2D<T> &other) const {
		return Vector2D<T>(X - other.X, Y - other.Y);
	}

	Vector2D<T> &operator-=(const Vector2D<T> &other)
	{
		X -= other.X;
		Y -= other.Y;
		return *this;
	}

	Vector2D<T> operator-(const T v) const {
		return Vector2D<T>(X - v, Y - v);
	}

	Vector2D<T> &operator-=(const T v)
	{
		X -= v;
		Y -= v;
		return *this;
	}

	//! Multiplication
	Vector2D<T> operator*(const Vector2D<T> &other) const {
		return Vector2D<T>(X * other.X, Y * other.Y);
	}

	Vector2D<T> &operator*=(const Vector2D<T> &other)
	{
		X *= other.X;
		Y *= other.Y;
		return *this;
	}

	Vector2D<T> operator*(const T v) const {
		return Vector2D<T>(X * v, Y * v);
	}

	Vector2D<T> &operator*=(const T v)
	{
		X *= v;
		Y *= v;
		return *this;
	}

	//! Division
	Vector2D<T> operator/(const Vector2D<T> &other) const {
		return Vector2D<T>(X / other.X, Y / other.Y);
	}

	Vector2D<T> &operator/=(const Vector2D<T> &other)
	{
		X /= other.X;
		Y /= other.Y;
		return *this;
	}

	Vector2D<T> operator/(const T v) const {
		return Vector2D<T>(X / v, Y / v);
	}

	Vector2D<T> &operator/=(const T v)
	{
		X /= v;
		Y /= v;
		return *this;
	}

	//! Less or equal comparison
	constexpr bool operator<=(const Vector2D<T> &other) const
	{
		return !(*this > other);
	}

	//! Greater or equal comparison
	constexpr bool operator>=(const Vector2D<T> &other) const
	{
		return !(*this < other);
	}

	//! Less comparison
	constexpr bool operator<(const Vector2D<T> &other) const
	{
		return X < other.X || (X == other.X && Y < other.Y);
	}

	//! Greater comparison
	constexpr bool operator>(const Vector2D<T> &other) const
	{
		return X > other.X || (X == other.X && Y > other.Y);
	}

	//! Equality
	constexpr bool operator==(const Vector2D<T> &other) const
	{
		return equals(X, other.X) && equals(Y, other.Y);
	}

	//! Inequality
	constexpr bool operator!=(const Vector2D<T> &other) const
	{
		return !(*this == other);
	}

	// functions

	//! Gets the length of the vector.
	/** \return The length of the vector. */
	T getLength() const { return std::sqrt(X * X + Y * Y); }

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	T getLengthSQ() const { return X * X + Y * Y; }

	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	T dotProduct(const Vector2D<T> &other) const
	{
		return X * other.X + Y * other.Y;
	}

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	T getDistanceFrom(const Vector2D<T> &other) const
	{
		return (*this - other).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	T getDistanceFromSQ(const Vector2D<T> &other) const
	{
		return (*this - other).getLengthSQ();
	}

	//! rotates the point anticlockwise around a center by an amount of degrees.
	/** \param degrees Amount of degrees to rotate by, anticlockwise.
	\param center Rotation center.
	\return This vector after transformation. */
	Vector2D<T> &rotateBy(f64 degrees, const Vector2D<T> &center = Vector2D<T>())
	{
		f64 radians = degToRad(degrees);
		const f64 cs = std::cos(radians);
		const f64 sn = std::sin(radians);

		X -= center.X;
		Y -= center.Y;

		X = (T)(X * cs - Y * sn);
		Y = (T)(X * sn + Y * cs);

		X += center.X;
		Y += center.Y;
		return *this;
	}

	//! Normalize the vector.
	/** The null vector is left untouched.
	\return Reference to this vector, after normalization. */
	Vector2D<T> &normalize()
	{
		f32 length = (f32)getLengthSQ(*this);
		if (length == 0)
			return *this;
		length = 1.0f / std::sqrt(length);
		X = (T)(X * length);
		Y = (T)(Y * length);
		return *this;
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const Vector2D<T> &begin, const Vector2D<T> &end) const
	{
		const T dist = begin.getDistanceFromSQ(end);
		return getDistanceFromSQ(begin) <= dist &&
				getDistanceFromSQ(end) <= dist;
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	Vector2D<T> getInterpolated(const Vector2D<T> &other, f64 d) const
	{
		const f64 inv = 1.0f - d;
		return Vector2D<T>((T)(other.X * inv + X * d), (T)(other.Y * inv + Y * d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	Vector2D<T> getInterpolated_quadratic(const Vector2D<T> &v2, const Vector2D<T> &v3, f64 d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const f64 inv = 1.0f - d;
		const f64 mul0 = inv * inv;
		const f64 mul1 = 2.0f * d * inv;
		const f64 mul2 = d * d;

		return Vector2D<T>((T)(X * mul0 + v2.X * mul1 + v3.X * mul2),
				(T)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2));
	}

	/*! Test if this point and another 2 points taken as triplet
		are colinear, clockwise, anticlockwise. This can be used also
		to check winding order in triangles for 2D meshes.
		\return 0 if points are colinear, 1 if clockwise, 2 if anticlockwise
	*/
	s32 checkOrientation(const Vector2D<T> &b, const Vector2D<T> &c) const
	{
		// Example of clockwise points
		//
		//   ^ Y
		//   |       A
		//   |      . .
		//   |     .   .
		//   |    C.....B
		//   +---------------> X


		T val = (b.Y - Y) * (c.X - b.X) -
				(b.X - X) * (c.Y - b.Y);

		if (val == 0)
			return 0; // colinear

		return (val > 0) ? 1 : 2; // clock or counterclock wise
	}

	/*! Returns true if points (a,b,c) are clockwise on the X,Y plane*/
	inline bool areClockwise(const Vector2D<T> &b, const Vector2D<T> &c) const
	{
		T val = (b.Y - Y) * (c.X - b.X) -
				(b.X - X) * (c.Y - b.Y);

		return val > 0;
	}

	/*! Returns true if points (a,b,c) are counterclockwise on the X,Y plane*/
	inline bool areCounterClockwise(const Vector2D<T> &b, const Vector2D<T> &c) const
	{
		T val = (b.Y - Y) * (c.X - b.X) -
				(b.X - X) * (c.Y - b.Y);

		return val < 0;
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	Vector2D<T> &interpolate(const Vector2D<T> &a, const Vector2D<T> &b, f64 d)
	{
		X = (T)((f64)b.X + ((a.X - b.X) * d));
		Y = (T)((f64)b.Y + ((a.Y - b.Y) * d));
		return *this;
	}
};

typedef Vector2D<f32> v2f;
typedef Vector2D<s32> v2i;

}
