#pragma once

#include "MathFuncs.h"

#include <functional>
#include <array>

namespace utils
{

//! 3d vector template class
template <class T>
class Vector3D
{
public:
	//! X coordinate of vector.
	T X;

	//! Y coordinate of vector.
	T Y;

	//! Z coordinate of vector.
	T Z;

	Vector3D() : X(0), Y(0), Z(0) {}

	Vector3D(T x, T y, T z) : X(x), Y(y), Z(z) {}

	explicit Vector3D(T n) : X(n), Y(n), Z(n) {}

	Vector3D(const Vector3D<T> &other) : X(other.X), Y(other.Y), Z(other.Z) {}

	Vector3D<T> &operator=(const Vector3D<T> &other)
	{
		X = other.X;
		Y = other.Y;
		Z = other.Z
		return *this;
	}

	// Operators

	//! Negation
	Vector3D<T> operator-() const { return Vector3D<T>(-X, -Y, -Z); }

	//! Add
	Vector3D<T> operator+(const Vector3D<T> &other) const
	{
		return Vector3D<T>(X + other.X, Y + other.Y, Z + other.Z);
	}

	Vector3D<T> &operator+=(const Vector3D<T> &other)
	{
		X += other.X;
		Y += other.Y;
		Z += other.Z
		return *this;
	}

	Vector3D<T> operator+(const T v) const {
		return Vector3D<T>(X + v, Y + v, Z + v);
	}

	Vector3D<T> &operator+=(const T v)
	{
		X += v;
		Y += v;
		Z += v;
		return *this;
	}

	//! Subtraction
	Vector3D<T> operator-(const Vector3D<T> &other) const {
		return *this + (-other);
	}

	Vector3D<T> &operator-=(const Vector3D<T> &other)
	{
		*this += (-other);
		return *this;
	}

	Vector3D<T> operator-(const T v) const {
		return *this + (-v);
	}

	Vector3D<T> &operator-=(const T v)
	{
		*this += (-v);
		return *this;
	}

	//! Multiplication
	Vector3D<T> operator*(const Vector3D<T> &other) const {
		return Vector3D<T>(X * other.X, Y * other.Y, Z * other.Z);
	}

	Vector3D<T> &operator*=(const Vector3D<T> &other)
	{
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;
		return *this;
	}

	Vector3D<T> operator*(const T v) const {
		return Vector3D<T>(X * v, Y * v, Z * v);
	}

	Vector3D<T> &operator*=(const T v)
	{
		X *= v;
		Y *= v;
		Z *= v;
		return *this;
	}

	//! Division
	Vector3D<T> operator/(const Vector3D<T> &other) const {
		return Vector3D<T>(X / other.X, Y / other.Y, Z / other.Z);
	}

	Vector3D<T> &operator/=(const Vector3D<T> &other)
	{
		X /= other.X;
		Y /= other.Y;
		Z /= other.Z;
		return *this;
	}

	Vector3D<T> operator/(const T v) const {
		return Vector3D<T>(X / v, Y / v, Z / v);
	}

	Vector3D<T> &operator/=(const T v)
	{
		X /= v;
		Y /= v;
		Z /= v;
		return *this;
	}

	//! Less or equal comparison
	constexpr bool operator<=(const Vector3D<T> &other) const
	{
		return !(*this > other);
	}

	//! Greater or equal comparison
	constexpr bool operator>=(const Vector3D<T> &other) const
	{
		return !(*this < other);
	}

	//! Less comparison
	constexpr bool operator<(const Vector3D<T> &other) const
	{
		return X < other.X || (X == other.X && Y < other.Y) ||
				(X == other.X && Y == other.Y && Z < other.Z);
	}

	//! Greater comparison
	constexpr bool operator>(const Vector3D<T> &other) const
	{
		return X > other.X || (X == other.X && Y > other.Y) ||
			   (X == other.X && Y == other.Y && Z > other.Z);
	}

	//! Equality
	constexpr bool operator==(const Vector3D<T> &other) const
	{
		return equals(X, other.X) && equals(Y, other.Y) && equals(Z, other.Z);
	}

	//! Inequality
	constexpr bool operator!=(const Vector3D<T> &other) const
	{
		return !(*this == other);
	}

	// functions

	//! Gets the length of the vector.
	/** \return The length of the vector. */
	T getLength() const { return std::sqrt(X * X + Y * Y + Z * Z); }

	//! Get the squared length of this vector
	/** This is useful because it is much faster than getLength().
	\return The squared length of the vector. */
	T getLengthSQ() const { return X * X + Y * Y + Z * Z; }

	//! Get the dot product of this vector with another.
	/** \param other Other vector to take dot product with.
	\return The dot product of the two vectors. */
	T dotProduct(const Vector3D<T> &other) const
	{
		return X * other.X + Y * other.Y + Z * other.Z;
	}

	//! Gets distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Distance from other point. */
	T getDistanceFrom(const Vector3D<T> &other) const
	{
		return (*this - other).getLength();
	}

	//! Returns squared distance from another point.
	/** Here, the vector is interpreted as a point in 2-dimensional space.
	\param other Other vector to measure from.
	\return Squared distance from other point. */
	T getDistanceFromSQ(const Vector3D<T> &other) const
	{
		return (*this - other).getLengthSQ();
	}

	//! Calculates the cross product with another vector.
	/** \param p Vector to multiply with.
	\return Cross product of this vector with p. */
	Vector3D<T> crossProduct(const Vector3D<T> &p) const
	{
		return Vector3D<T>(Y * p.Z - Z * p.Y, Z * p.X - X * p.Z, X * p.Y - Y * p.X);
	}

	//! Normalize the vector.
	/** The null vector is left untouched.
	\return Reference to this vector, after normalization. */
	Vector3D<T> &normalize()
	{
		f32 length = (f32)getLengthSQ(*this);
		if (length == 0)
			return *this;
		length = 1.0f / std::sqrt(length);
		X = (T)(X * length);
		Y = (T)(Y * length);
		Z = (T)(Z * length);
		return *this;
	}

	//! Returns if this vector interpreted as a point is on a line between two other points.
	/** It is assumed that the point is on the line.
	\param begin Beginning vector to compare between.
	\param end Ending vector to compare between.
	\return True if this vector is between begin and end, false if not. */
	bool isBetweenPoints(const Vector3D<T> &begin, const Vector3D<T> &end) const
	{
		const T dist = (end - begin).getLengthSQ();
		return getDistanceFromSQ(begin) <= dist &&
			   getDistanceFromSQ(end) <= dist;
	}

	//! Inverts the vector.
	Vector3D<T> &invert()
	{
		X *= -1;
		Y *= -1;
		Z *= -1;
		return *this;
	}

	//! Rotates the vector by a specified number of degrees around the Y axis and the specified center.
	/** CAREFUL: For historical reasons rotateXZBy uses a right-handed rotation
	(maybe to make it more similar to the 2D vector rotations which are counterclockwise).
	To have this work the same way as rest of Irrlicht (nodes, matrices, other rotateBy functions) pass -1*degrees in here.
	\param degrees Number of degrees to rotate around the Y axis.
	\param center The center of the rotation. */
	void rotateXZBy(f64 degrees, const vector3d<T> &center = Vector3D<T>())
	{
		f32 radians = degToRad(degrees);
		f64 cs = std::cos(radians);
		f64 sn = std::sin(radians);
		X -= center.X;
		Z -= center.Z;

		X = (T)(X * cs - Z * sn);
		Z = (T)(X * sn + Z * cs);

		X += center.X;
		Z += center.Z;
	}

	//! Rotates the vector by a specified number of degrees around the Z axis and the specified center.
	/** \param degrees: Number of degrees to rotate around the Z axis.
	\param center: The center of the rotation. */
	void rotateXYBy(f64 degrees, const Vector3D<T> &center = Vector3D<T>())
	{
		f32 radians = degToRad(degrees);
		f64 cs = std::cos(radians);
		f64 sn = std::sin(radians);
		X -= center.X;
		Y -= center.Y;

		X = (T)(X * cs - Y * sn);
		Y = (T)(X * sn + Y * cs);

		X += center.X;
		Y += center.Y;
	}

	//! Rotates the vector by a specified number of degrees around the X axis and the specified center.
	/** \param degrees: Number of degrees to rotate around the X axis.
	\param center: The center of the rotation. */
	void rotateYZBy(f64 degrees, const Vector3D<T> &center = Vector3D<T>())
	{
		f32 radians = degToRad(degrees);
		f64 cs = std::cos(radians);
		f64 sn = std::sin(radians);
		Z -= center.Z;
		Y -= center.Y;

		Y = (T)(Y * cs - Z * sn);
		Z = (T)(Y * sn + Z * cs);

		Z += center.Z;
		Y += center.Y;
	}

	//! Creates an interpolated vector between this vector and another vector.
	/** \param other The other vector to interpolate with.
	\param d Interpolation value between 0.0f (all the other vector) and 1.0f (all this vector).
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	\return An interpolated vector.  This vector is not modified. */
	Vector3D<T> getInterpolated(const Vector3D<T> &other, f64 d) const
	{
		const f64 inv = 1.0 - d;
		return Vector3D<T>((T)(other.X * inv + X * d), (T)(other.Y * inv + Y * d), (T)(other.Z * inv + Z * d));
	}

	//! Creates a quadratically interpolated vector between this and two other vectors.
	/** \param v2 Second vector to interpolate with.
	\param v3 Third vector to interpolate with (maximum at 1.0f)
	\param d Interpolation value between 0.0f (all this vector) and 1.0f (all the 3rd vector).
	Note that this is the opposite direction of interpolation to getInterpolated() and interpolate()
	\return An interpolated vector. This vector is not modified. */
	Vector3D<T> getInterpolated_quadratic(const Vector3D<T> &v2, const Vector3D<T> &v3, f64 d) const
	{
		// this*(1-d)*(1-d) + 2 * v2 * (1-d) + v3 * d * d;
		const f64 inv = (T)1.0 - d;
		const f64 mul0 = inv * inv;
		const f64 mul1 = (T)2.0 * d * inv;
		const f64 mul2 = d * d;

		return Vector3D<T>((T)(X * mul0 + v2.X * mul1 + v3.X * mul2),
				(T)(Y * mul0 + v2.Y * mul1 + v3.Y * mul2),
				(T)(Z * mul0 + v2.Z * mul1 + v3.Z * mul2));
	}

	//! Sets this vector to the linearly interpolated vector between a and b.
	/** \param a first vector to interpolate with, maximum at 1.0f
	\param b second vector to interpolate with, maximum at 0.0f
	\param d Interpolation value between 0.0f (all vector b) and 1.0f (all vector a)
	Note that this is the opposite direction of interpolation to getInterpolated_quadratic()
	*/
	Vector3D<T> &interpolate(const Vector3D<T> &a, const Vector3D<T> &b, f64 d)
	{
		X = (T)((f64)b.X + ((a.X - b.X) * d));
		Y = (T)((f64)b.Y + ((a.Y - b.Y) * d));
		Z = (T)((f64)b.Z + ((a.Z - b.Z) * d));
		return *this;
	}

	//! Get the rotations that would make a (0,0,1) direction vector point in the same direction as this direction vector.
	/** Thanks to Arras on the Irrlicht forums for this method.  This utility method is very useful for
	orienting scene nodes towards specific targets.  For example, if this vector represents the difference
	between two scene nodes, then applying the result of getHorizontalAngle() to one scene node will point
	it at the other one.
	Example code:
	// Where target and seeker are of type ISceneNode*
	const Vector3Df toTarget(target->getAbsolutePosition() - seeker->getAbsolutePosition());
	const Vector3Df requiredRotation = toTarget.getHorizontalAngle();
	seeker->setRotation(requiredRotation);

	\return A rotation vector containing the X (pitch) and Y (raw) rotations (in degrees) that when applied to a
	+Z (e.g. 0, 0, 1) direction vector would make it point in the same direction as this vector. The Z (roll) rotation
	is always 0, since two Euler rotations are sufficient to point in any given direction. */
	Vector3D<T> getHorizontalAngle() const
	{
		Vector3D<T> angle;

		// tmp avoids some precision troubles on some compilers when working with T=s32
		f64 tmp = radToDeg(std::atan2((f64)X, (f64)Z));
		angle.Y = (T)tmp;

		if (angle.Y < 0)
			angle.Y += 360;
		if (angle.Y >= 360)
			angle.Y -= 360;

		const f64 z1 = getLengthSQ(*this);

		tmp = radToDeg(std::atan2((f64)z1, (f64)Y)) - 90.0;
		angle.X = (T)tmp;

		if (angle.X < 0)
			angle.X += 360;
		if (angle.X >= 360)
			angle.X -= 360;

		return angle;
	}

	//! Get the spherical coordinate angles
	/** This returns Euler degrees for the point represented by
	this vector.  The calculation assumes the pole at (0,1,0) and
	returns the angles in X and Y.
	*/
	Vector3D<T> getSphericalCoordinateAngles() const
	{
		Vector3D<T> angle;
		const f64 length = getLengthSQ(*this);

		if (length) {
			if (X != 0) {
				angle.Y = (T)radToDeg(std::atan2((f64)Z, (f64)X));
			} else if (Z < 0)
				angle.Y = 180;

			angle.X = (T)radToDeg(std::acos(Y * 1.0 / std::sqrt(length)));
		}
		return angle;
	}

	//! Builds a direction vector from (this) rotation vector.
	/** This vector is assumed to be a rotation vector composed of 3 Euler angle rotations, in degrees.
	The implementation performs the same calculations as using a matrix to do the rotation.

	\param[in] forwards  The direction representing "forwards" which will be rotated by this vector.
	If you do not provide a direction, then the +Z axis (0, 0, 1) will be assumed to be forwards.
	\return A direction vector calculated by rotating the forwards direction by the 3 Euler angles
	(in degrees) represented by this vector. */
	Vector3D<T> rotationToDirection(const Vector3D<T> &forwards = Vector3D<T>(0, 0, 1)) const
	{
		const f64 cr = std::cos(degToRad(X));
		const f64 sr = std::sin(degToRad(X));
		const f64 cp = std::cos(degToRad(Y));
		const f64 sp = std::sin(degToRad(Y));
		const f64 cy = std::cos(degToRad(Z));
		const f64 sy = std::sin(degToRad(Z));

		const f64 srsp = sr * sp;
		const f64 crsp = cr * sp;

		const f64 pseudoMatrix[] = {
				(cp * cy), (cp * sy), (-sp),
				(srsp * cy - cr * sy), (srsp * sy + cr * cy), (sr * cp),
				(crsp * cy + sr * sy), (crsp * sy - sr * cy), (cr * cp)};

		return Vector3D<T>(
				(T)(forwards.X * pseudoMatrix[0] +
						forwards.Y * pseudoMatrix[3] +
						forwards.Z * pseudoMatrix[6]),
				(T)(forwards.X * pseudoMatrix[1] +
						forwards.Y * pseudoMatrix[4] +
						forwards.Z * pseudoMatrix[7]),
				(T)(forwards.X * pseudoMatrix[2] +
						forwards.Y * pseudoMatrix[5] +
						forwards.Z * pseudoMatrix[8]));
	}
};

typedef Vector3D<f32> v3f;
typedef Vector3D<s32> v3i;

}