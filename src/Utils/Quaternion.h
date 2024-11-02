// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Typedefs.h"
#include "MathFuncs.h"
#include "Matrix4.h"
#include "Vector3D.h"


namespace utils
{

//! Quaternion class for representing rotations.
/** It provides cheap combinations and avoids gimbal locks.
Also useful for interpolations. */
class Quaternion
{
public:
	//! Quaternion elements.
	f32 X; // vectorial (imaginary) part
	f32 Y;
	f32 Z;
	f32 W; // real part
	
	//! Default Constructor
	Quaternion() :
			X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}

	//! Constructor
	Quaternion(f32 x, f32 y, f32 z, f32 w) :
			X(x), Y(y), Z(z), W(w) {}

	//! Constructor which converts Euler angles (radians) to a Quaternion
	Quaternion(f32 x, f32 y, f32 z);

	//! Constructor which converts Euler angles (radians) to a Quaternion
	Quaternion(const v3f &vec);

	//! Equality operator
	bool operator==(const Quaternion &other) const
	{
		return ((X == other.X) &&
				(Y == other.Y) &&
				(Z == other.Z) &&
				(W == other.W));
	}

	//! inequality operator
	bool operator!=(const Quaternion &other) const
	{
		return !(*this == other);
	}

	//! Add operator
	Quaternion operator+(const Quaternion &other) const;

	//! Multiplication operator
	//! Be careful, unfortunately the operator order here is opposite of that in Matrix4::operator*
	Quaternion operator*(const Quaternion &other) const;

	//! Multiplication operator with scalar
	Quaternion operator*(f32 s) const;

	//! Multiplication operator with scalar
	Quaternion &operator*=(f32 s);

	//! Multiplication operator
	v3f operator*(const v3f &v) const;

	//! Multiplication operator
	Quaternion &operator*=(const Quaternion &other);

	//! Calculates the dot product
	inline f32 dotProduct(const Quaternion &other) const;

	//! Sets new Quaternion
	inline Quaternion &set(f32 x, f32 y, f32 z, f32 w);

	//! Sets new Quaternion based on Euler angles (radians)
	inline Quaternion &set(f32 x, f32 y, f32 z);

	//! Sets new Quaternion based on Euler angles (radians)
	inline Quaternion &set(const v3f &vec);

	//! Sets new Quaternion from other Quaternion
	inline Quaternion &set(const utils::Quaternion &quat);

	//! returns if this Quaternion equals the other one, taking floating point rounding errors into account
	inline bool equals(const Quaternion &other,
			const f32 tolerance = ROUNDING_ERROR_f32) const;

	//! Normalizes the Quaternion
	inline Quaternion &normalize();

	//! Faster method to create a rotation matrix, you should normalize the Quaternion before!
	void getMatrixFast(matrix4 &dest) const;

	//! Creates a matrix from this Quaternion
	void getMatrix(matrix4 &dest, const v3f &translation = v3f()) const;

	/*!
		Creates a matrix from this Quaternion
		Rotate about a center point
		shortcut for
		utils::Quaternion q;
		q.rotationFromTo ( vin[i].Normal, forward );
		q.getMatrixCenter ( lookat, center, newPos );

		utils::matrix4 m2;
		m2.setInverseTranslation ( center );
		lookat *= m2;

		utils::matrix4 m3;
		m2.setTranslation ( newPos );
		lookat *= m3;

	*/
	void getMatrixCenter(matrix4 &dest, const v3f &center, const v3f &translation) const;

	//! Creates a matrix from this Quaternion
	inline void getMatrix_transposed(matrix4 &dest) const;

	//! Inverts this Quaternion
	Quaternion &makeInverse();

	//! Set this Quaternion to the linear interpolation between two Quaternions
	/** NOTE: lerp result is *not* a normalized Quaternion. In most cases
	you will want to use lerpN instead as most other Quaternion functions expect
	to work with a normalized Quaternion.
	\param q1 First Quaternion to be interpolated.
	\param q2 Second Quaternion to be interpolated.
	\param time Progress of interpolation. For time=0 the result is
	q1, for time=1 the result is q2. Otherwise interpolation
	between q1 and q2. Result is not normalized.
	*/
	Quaternion &lerp(Quaternion q1, Quaternion q2, f32 time);

	//! Set this Quaternion to the linear interpolation between two Quaternions and normalize the result
	/**
	\param q1 First Quaternion to be interpolated.
	\param q2 Second Quaternion to be interpolated.
	\param time Progress of interpolation. For time=0 the result is
	q1, for time=1 the result is q2. Otherwise interpolation
	between q1 and q2. Result is normalized.
	*/
	Quaternion &lerpN(Quaternion q1, Quaternion q2, f32 time);

	//! Set this Quaternion to the result of the spherical interpolation between two Quaternions
	/** \param q1 First Quaternion to be interpolated.
	\param q2 Second Quaternion to be interpolated.
	\param time Progress of interpolation. For time=0 the result is
	q1, for time=1 the result is q2. Otherwise interpolation
	between q1 and q2.
	\param threshold To avoid inaccuracies at the end (time=1) the
	interpolation switches to linear interpolation at some point.
	This value defines how much of the remaining interpolation will
	be calculated with lerp. Everything from 1-threshold up will be
	linear interpolation.
	*/
	Quaternion &slerp(Quaternion q1, Quaternion q2,
			f32 time, f32 threshold = .05f);

	//! Set this Quaternion to represent a rotation from angle and axis.
	/** Axis must be unit length.
	The Quaternion representing the rotation is
	q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k).
	\param angle Rotation Angle in radians.
	\param axis Rotation axis. */
	Quaternion &fromAngleAxis(f32 angle, const v3f &axis);

	//! Fills an angle (radians) around an axis (unit vector)
	void toAngleAxis(f32 &angle, v3f &axis) const;

	//! Output this Quaternion to an Euler angle (radians)
	void toEuler(v3f &euler) const;

	//! Set Quaternion to identity
	Quaternion &makeIdentity();

	//! Set Quaternion to represent a rotation from one vector to another.
	Quaternion &rotationFromTo(const v3f &from, const v3f &to);
};

// Constructor which converts Euler angles to a Quaternion
inline Quaternion::Quaternion(f32 x, f32 y, f32 z)
{
	set(x, y, z);
}

// Constructor which converts Euler angles to a Quaternion
inline Quaternion::Quaternion(const v3f &vec)
{
	set(vec.X, vec.Y, vec.Z);
}

// multiplication operator
inline Quaternion Quaternion::operator*(const Quaternion &other) const
{
	Quaternion tmp;

	tmp.W = (other.W * W) - (other.X * X) - (other.Y * Y) - (other.Z * Z);
	tmp.X = (other.W * X) + (other.X * W) + (other.Y * Z) - (other.Z * Y);
	tmp.Y = (other.W * Y) + (other.Y * W) + (other.Z * X) - (other.X * Z);
	tmp.Z = (other.W * Z) + (other.Z * W) + (other.X * Y) - (other.Y * X);

	return tmp;
}

// multiplication operator
inline Quaternion Quaternion::operator*(f32 s) const
{
	return Quaternion(s * X, s * Y, s * Z, s * W);
}

// multiplication operator
inline Quaternion &Quaternion::operator*=(f32 s)
{
	X *= s;
	Y *= s;
	Z *= s;
	W *= s;
	return *this;
}

// multiplication operator
inline Quaternion &Quaternion::operator*=(const Quaternion &other)
{
	return (*this = other * (*this));
}

// add operator
inline Quaternion Quaternion::operator+(const Quaternion &b) const
{
	return Quaternion(X + b.X, Y + b.Y, Z + b.Z, W + b.W);
}

//! Faster method to create a rotation matrix, you should normalize the Quaternion before!
inline void Quaternion::getMatrixFast(matrix4 &dest) const
{
	// TODO:
	// gpu Quaternion skinning => fast Bones transform chain O_O YEAH!
	// http://www.mrelusive.com/publications/papers/SIMD-From-Quaternion-to-Matrix-and-Back.pdf
	dest[0] = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
	dest[1] = 2.0f * X * Y + 2.0f * Z * W;
	dest[2] = 2.0f * X * Z - 2.0f * Y * W;
	dest[3] = 0.0f;

	dest[4] = 2.0f * X * Y - 2.0f * Z * W;
	dest[5] = 1.0f - 2.0f * X * X - 2.0f * Z * Z;
	dest[6] = 2.0f * Z * Y + 2.0f * X * W;
	dest[7] = 0.0f;

	dest[8] = 2.0f * X * Z + 2.0f * Y * W;
	dest[9] = 2.0f * Z * Y - 2.0f * X * W;
	dest[10] = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
	dest[11] = 0.0f;

	dest[12] = 0.f;
	dest[13] = 0.f;
	dest[14] = 0.f;
	dest[15] = 1.f;

	dest.setDefinitelyIdentityMatrix(false);
}

/*!
	Creates a matrix from this Quaternion
*/
inline void Quaternion::getMatrix(matrix4 &dest,
		const v3f &center) const
{
	// ok creating a copy may be slower, but at least avoid internal
	// state chance (also because otherwise we cannot keep this method "const").

	Quaternion q(*this);
	q.normalize();
	f32 X = q.X;
	f32 Y = q.Y;
	f32 Z = q.Z;
	f32 W = q.W;

	dest[0] = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
	dest[1] = 2.0f * X * Y + 2.0f * Z * W;
	dest[2] = 2.0f * X * Z - 2.0f * Y * W;
	dest[3] = 0.0f;

	dest[4] = 2.0f * X * Y - 2.0f * Z * W;
	dest[5] = 1.0f - 2.0f * X * X - 2.0f * Z * Z;
	dest[6] = 2.0f * Z * Y + 2.0f * X * W;
	dest[7] = 0.0f;

	dest[8] = 2.0f * X * Z + 2.0f * Y * W;
	dest[9] = 2.0f * Z * Y - 2.0f * X * W;
	dest[10] = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
	dest[11] = 0.0f;

	dest[12] = center.X;
	dest[13] = center.Y;
	dest[14] = center.Z;
	dest[15] = 1.f;

	dest.setDefinitelyIdentityMatrix(false);
}

/*!
	Creates a matrix from this Quaternion
	Rotate about a center point
	shortcut for
	utils::Quaternion q;
	q.rotationFromTo(vin[i].Normal, forward);
	q.getMatrix(lookat, center);

	utils::matrix4 m2;
	m2.setInverseTranslation(center);
	lookat *= m2;
*/
inline void Quaternion::getMatrixCenter(matrix4 &dest,
		const v3f &center,
		const v3f &translation) const
{
	Quaternion q(*this);
	q.normalize();
	f32 X = q.X;
	f32 Y = q.Y;
	f32 Z = q.Z;
	f32 W = q.W;

	dest[0] = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
	dest[1] = 2.0f * X * Y + 2.0f * Z * W;
	dest[2] = 2.0f * X * Z - 2.0f * Y * W;
	dest[3] = 0.0f;

	dest[4] = 2.0f * X * Y - 2.0f * Z * W;
	dest[5] = 1.0f - 2.0f * X * X - 2.0f * Z * Z;
	dest[6] = 2.0f * Z * Y + 2.0f * X * W;
	dest[7] = 0.0f;

	dest[8] = 2.0f * X * Z + 2.0f * Y * W;
	dest[9] = 2.0f * Z * Y - 2.0f * X * W;
	dest[10] = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
	dest[11] = 0.0f;

	dest.setRotationCenter(center, translation);
}

// Creates a matrix from this Quaternion
inline void Quaternion::getMatrix_transposed(matrix4 &dest) const
{
	Quaternion q(*this);
	q.normalize();
	f32 X = q.X;
	f32 Y = q.Y;
	f32 Z = q.Z;
	f32 W = q.W;

	dest[0] = 1.0f - 2.0f * Y * Y - 2.0f * Z * Z;
	dest[4] = 2.0f * X * Y + 2.0f * Z * W;
	dest[8] = 2.0f * X * Z - 2.0f * Y * W;
	dest[12] = 0.0f;

	dest[1] = 2.0f * X * Y - 2.0f * Z * W;
	dest[5] = 1.0f - 2.0f * X * X - 2.0f * Z * Z;
	dest[9] = 2.0f * Z * Y + 2.0f * X * W;
	dest[13] = 0.0f;

	dest[2] = 2.0f * X * Z + 2.0f * Y * W;
	dest[6] = 2.0f * Z * Y - 2.0f * X * W;
	dest[10] = 1.0f - 2.0f * X * X - 2.0f * Y * Y;
	dest[14] = 0.0f;

	dest[3] = 0.f;
	dest[7] = 0.f;
	dest[11] = 0.f;
	dest[15] = 1.f;

	dest.setDefinitelyIdentityMatrix(false);
}

// Inverts this Quaternion
inline Quaternion &Quaternion::makeInverse()
{
	X = -X;
	Y = -Y;
	Z = -Z;
	return *this;
}

// sets new Quaternion
inline Quaternion &Quaternion::set(f32 x, f32 y, f32 z, f32 w)
{
	X = x;
	Y = y;
	Z = z;
	W = w;
	return *this;
}

// sets new Quaternion based on Euler angles
inline Quaternion &Quaternion::set(f32 x, f32 y, f32 z)
{
	f64 angle;

	angle = x * 0.5;
	const f64 sr = sin(angle);
	const f64 cr = cos(angle);

	angle = y * 0.5;
	const f64 sp = sin(angle);
	const f64 cp = cos(angle);

	angle = z * 0.5;
	const f64 sy = sin(angle);
	const f64 cy = cos(angle);

	const f64 cpcy = cp * cy;
	const f64 spcy = sp * cy;
	const f64 cpsy = cp * sy;
	const f64 spsy = sp * sy;

	X = (f32)(sr * cpcy - cr * spsy);
	Y = (f32)(cr * spcy + sr * cpsy);
	Z = (f32)(cr * cpsy - sr * spcy);
	W = (f32)(cr * cpcy + sr * spsy);

	return normalize();
}

// sets new Quaternion based on Euler angles
inline Quaternion &Quaternion::set(const v3f &vec)
{
	return set(vec.X, vec.Y, vec.Z);
}

// sets new Quaternion based on other Quaternion
inline Quaternion &Quaternion::set(const utils::Quaternion &quat)
{
	return (*this = quat);
}

//! returns if this Quaternion equals the other one, taking floating point rounding errors into account
inline bool Quaternion::equals(const Quaternion &other, const f32 tolerance) const
{
	return utils::equals(X, other.X, tolerance) &&
		   utils::equals(Y, other.Y, tolerance) &&
		   utils::equals(Z, other.Z, tolerance) &&
		   utils::equals(W, other.W, tolerance);
}

// normalizes the Quaternion
inline Quaternion &Quaternion::normalize()
{
	// removed conditional branch since it may slow down and anyway the condition was
	// false even after normalization in some cases.
	return (*this *= (f32)reciprocal_squareroot((f64)(X * X + Y * Y + Z * Z + W * W)));
}

// Set this Quaternion to the result of the linear interpolation between two Quaternions
inline Quaternion &Quaternion::lerp(Quaternion q1, Quaternion q2, f32 time)
{
	const f32 scale = 1.0f - time;
	return (*this = (q1 * scale) + (q2 * time));
}

// Set this Quaternion to the result of the linear interpolation between two Quaternions and normalize the result
inline Quaternion &Quaternion::lerpN(Quaternion q1, Quaternion q2, f32 time)
{
	const f32 scale = 1.0f - time;
	return (*this = ((q1 * scale) + (q2 * time)).normalize());
}

// set this Quaternion to the result of the interpolation between two Quaternions
inline Quaternion &Quaternion::slerp(Quaternion q1, Quaternion q2, f32 time, f32 threshold)
{
	f32 angle = q1.dotProduct(q2);

	// make sure we use the short rotation
	if (angle < 0.0f) {
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	if (angle <= (1 - threshold)) { // spherical interpolation
		const f32 theta = acosf(angle);
		const f32 invsintheta = reciprocal(sinf(theta));
		const f32 scale = sinf(theta * (1.0f - time)) * invsintheta;
		const f32 invscale = sinf(theta * time) * invsintheta;
		return (*this = (q1 * scale) + (q2 * invscale));
	} else // linear interpolation
		return lerpN(q1, q2, time);
}

// calculates the dot product
inline f32 Quaternion::dotProduct(const Quaternion &q2) const
{
	return (X * q2.X) + (Y * q2.Y) + (Z * q2.Z) + (W * q2.W);
}

//! axis must be unit length, angle in radians
inline Quaternion &Quaternion::fromAngleAxis(f32 angle, const v3f &axis)
{
	const f32 fHalfAngle = 0.5f * angle;
	const f32 fSin = sinf(fHalfAngle);
	W = cosf(fHalfAngle);
	X = fSin * axis.X;
	Y = fSin * axis.Y;
	Z = fSin * axis.Z;
	return *this;
}

inline void Quaternion::toAngleAxis(f32 &angle, v3f &axis) const
{
	const f32 scale = sqrtf(X * X + Y * Y + Z * Z);

	if (utils::iszero(scale) || W > 1.0f || W < -1.0f) {
		angle = 0.0f;
		axis.X = 0.0f;
		axis.Y = 1.0f;
		axis.Z = 0.0f;
	} else {
		const f32 invscale = reciprocal(scale);
		angle = 2.0f * acosf(W);
		axis.X = X * invscale;
		axis.Y = Y * invscale;
		axis.Z = Z * invscale;
	}
}

inline void Quaternion::toEuler(v3f &euler) const
{
	const f64 sqw = W * W;
	const f64 sqx = X * X;
	const f64 sqy = Y * Y;
	const f64 sqz = Z * Z;
	const f64 test = 2.0 * (Y * W - X * Z);

	if (utils::equals(test, 1.0, 0.000001)) {
		// heading = rotation about z-axis
		euler.Z = (f32)(-2.0 * atan2(X, W));
		// bank = rotation about x-axis
		euler.X = 0;
		// attitude = rotation about y-axis
		euler.Y = (f32)(utils::PI64 / 2.0);
	} else if (utils::equals(test, -1.0, 0.000001)) {
		// heading = rotation about z-axis
		euler.Z = (f32)(2.0 * atan2(X, W));
		// bank = rotation about x-axis
		euler.X = 0;
		// attitude = rotation about y-axis
		euler.Y = (f32)(utils::PI64 / -2.0);
	} else {
		// heading = rotation about z-axis
		euler.Z = (f32)atan2(2.0 * (X * Y + Z * W), (sqx - sqy - sqz + sqw));
		// bank = rotation about x-axis
		euler.X = (f32)atan2(2.0 * (Y * Z + X * W), (-sqx - sqy + sqz + sqw));
		// attitude = rotation about y-axis
		euler.Y = (f32)asin(clamp(test, -1.0, 1.0));
	}
}

inline v3f Quaternion::operator*(const v3f &v) const
{
	// nVidia SDK implementation

	v3f uv, uuv;
	const v3f qvec(X, Y, Z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * W);
	uuv *= 2.0f;

	return v + uv + uuv;
}

// set Quaternion to identity
inline utils::Quaternion &Quaternion::makeIdentity()
{
	W = 1.f;
	X = 0.f;
	Y = 0.f;
	Z = 0.f;
	return *this;
}

inline utils::Quaternion &Quaternion::rotationFromTo(const v3f &from, const v3f &to)
{
	// Based on Stan Melax's article in Game Programming Gems
	// Optimized by Robert Eisele: https://raw.org/proof/Quaternion-from-two-vectors

	// Copy, since cannot modify local
	v3f v0 = from;
	v3f v1 = to;
	v0.normalize();
	v1.normalize();

	const f32 d = v0.dotProduct(v1);
	if (d >= 1.0f) { // If dot == 1, vectors are the same
		return makeIdentity();
	} else if (d <= -1.0f) { // exactly opposite
		v3f axis(1.0f, 0.f, 0.f);
		axis = axis.crossProduct(v0);
		if (axis.getLength() == 0) {
			axis.set(0.f, 1.f, 0.f);
			axis = axis.crossProduct(v0);
		}
		// same as fromAngleAxis(utils::PI, axis).normalize();
		return set(axis.X, axis.Y, axis.Z, 0).normalize();
	}

	const v3f c = v0.crossProduct(v1);
	return set(c.X, c.Y, c.Z, 1 + d).normalize();
}

}
