// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "BasicIncludes.h"
#include "MathFuncs.h"
#include "Matrix4.h"

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
    f32 W=0.0f; // real part

	//! Default Constructor
	constexpr Quaternion() :
			X(0.0f), Y(0.0f), Z(0.0f), W(1.0f) {}

	//! Constructor
	constexpr Quaternion(f32 x, f32 y, f32 z, f32 w) :
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

    //! Set this Quaternion to represent the euler rotation
    void fromEuler(const v3f &euler);

	//! Set Quaternion to identity
	Quaternion &makeIdentity();

	//! Set Quaternion to represent a rotation from one vector to another.
	Quaternion &rotationFromTo(const v3f &from, const v3f &to);
};

}
