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
		return *this + (-other);
	}

	Vector2D<T> &operator-=(const Vector2D<T> &other)
	{
		return *this += (-other);
	}

	Vector2D<T> operator-(const T v) const {
		return *this + (-v);
	}

	Vector2D<T> &operator-=(const T v)
	{
		return *this += (-v);
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

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	Vector2D<T> linInterp(const Vector2D<T> &other, f32 d) const
	{
		return Vector2D<T>(lerp<T>(other.X, X, d), lerp<T>(other.Y, Y, d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	Vector2D<T> quadInterp(const Vector2D<T> &v2, const Vector2D<T> &v3, f32 d) const
	{
		return Vector2D<T>(qerp<T>(X, v2.X, v3.X, d), qerp<T>(Y, v2.Y, v3.Y, d));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	Vector2D<T> &linInterpBetween(const Vector2D<T> &a, const Vector2D<T> &b, f32 d)
	{
		X = lerp(a.X, b.X, d);
		Y = lerp(a.Y, b.Y, d);

		return *this;
	}
};

typedef Vector2D<f32> v2f;
typedef Vector2D<s32> v2i;

}
