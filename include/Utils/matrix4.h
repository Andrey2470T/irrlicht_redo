// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include <cstring> // memset, memcpy
#include "Utils/irrMath.h"
#include "Utils/vector3d.h"
#include "Utils/vector2d.h"
#include "plane3d.h"
#include "Utils/aabbox3d.h"
#include "Utils/rect.h"
#include <cassert>

namespace core
{

//! 4x4 matrix. Mostly used as transformation matrix for 3d calculations.
/** Conventions: Matrices are considered to be in row-major order.
 * Multiplication of a matrix A with a row vector v is the premultiplication vA.
 * Translations are thus in the 4th row.
 * The matrix product AB yields a matrix C such that vC = (vB)A:
 * B is applied first, then A.
 */
template <class T>
class CMatrix4
{
public:
	//! Constructor Flags
	enum eConstructor
	{
		EM4CONST_NOTHING = 0,
		EM4CONST_COPY,
		EM4CONST_IDENTITY,
		EM4CONST_TRANSPOSED,
		EM4CONST_INVERSE,
		EM4CONST_INVERSE_TRANSPOSED
	};

	//! Default constructor
	/** \param constructor Choose the initialization style */
	CMatrix4(eConstructor constructor = EM4CONST_IDENTITY);

	//! Constructor with value initialization
	constexpr CMatrix4(const T &r0c0, const T &r0c1, const T &r0c2, const T &r0c3,
			const T &r1c0, const T &r1c1, const T &r1c2, const T &r1c3,
			const T &r2c0, const T &r2c1, const T &r2c2, const T &r2c3,
			const T &r3c0, const T &r3c1, const T &r3c2, const T &r3c3)
	{
		M[0] = r0c0;
		M[1] = r0c1;
		M[2] = r0c2;
		M[3] = r0c3;
		M[4] = r1c0;
		M[5] = r1c1;
		M[6] = r1c2;
		M[7] = r1c3;
		M[8] = r2c0;
		M[9] = r2c1;
		M[10] = r2c2;
		M[11] = r2c3;
		M[12] = r3c0;
		M[13] = r3c1;
		M[14] = r3c2;
		M[15] = r3c3;
	}

	//! Copy constructor
	/** \param other Other matrix to copy from
	\param constructor Choose the initialization style */
	CMatrix4(const CMatrix4<T> &other, eConstructor constructor = EM4CONST_COPY);

	//! Simple operator for directly accessing every element of the matrix.
	T &operator()(const s32 row, const s32 col)
	{
		return M[row * 4 + col];
	}

	//! Simple operator for directly accessing every element of the matrix.
	const T &operator()(const s32 row, const s32 col) const { return M[row * 4 + col]; }

	//! Simple operator for linearly accessing every element of the matrix.
	T &operator[](u32 index)
	{
		return M[index];
	}

	//! Simple operator for linearly accessing every element of the matrix.
	const T &operator[](u32 index) const { return M[index]; }

	//! Sets this matrix equal to the other matrix.
	CMatrix4<T> &operator=(const CMatrix4<T> &other) = default;

	//! Sets all elements of this matrix to the value.
	inline CMatrix4<T> &operator=(const T &scalar);

	//! Returns pointer to internal array
	const T *pointer() const { return M; }
	T *pointer()
	{
		return M;
	}

	//! Returns true if other matrix is equal to this matrix.
	constexpr bool operator==(const CMatrix4<T> &other) const
	{
		for (s32 i = 0; i < 16; ++i)
			if (M[i] != other.M[i])
				return false;

		return true;
	}

	//! Returns true if other matrix is not equal to this matrix.
	constexpr bool operator!=(const CMatrix4<T> &other) const
	{
		return !(*this == other);
	}

	//! Add another matrix.
	CMatrix4<T> operator+(const CMatrix4<T> &other) const;

	//! Add another matrix.
	CMatrix4<T> &operator+=(const CMatrix4<T> &other);

	//! Subtract another matrix.
	CMatrix4<T> operator-(const CMatrix4<T> &other) const;

	//! Subtract another matrix.
	CMatrix4<T> &operator-=(const CMatrix4<T> &other);

	//! set this matrix to the product of two matrices
	/** Calculate b*a */
	inline CMatrix4<T> &setbyproduct(const CMatrix4<T> &other_a, const CMatrix4<T> &other_b);

	//! Set this matrix to the product of two matrices
	/** Calculate b*a, no optimization used,
	use it if you know you never have an identity matrix */
	CMatrix4<T> &setbyproduct_nocheck(const CMatrix4<T> &other_a, const CMatrix4<T> &other_b);

	//! Multiply by another matrix.
	/** Calculate other*this */
	CMatrix4<T> operator*(const CMatrix4<T> &other) const;

	//! Multiply by another matrix.
	/** Like calling: (*this) = (*this) * other
	 */
	CMatrix4<T> &operator*=(const CMatrix4<T> &other);

	//! Multiply by scalar.
	CMatrix4<T> operator*(const T &scalar) const;

	//! Multiply by scalar.
	CMatrix4<T> &operator*=(const T &scalar);

	//! Set matrix to identity.
	inline CMatrix4<T> &makeIdentity();

	//! Returns true if the matrix is the identity matrix
	inline bool isIdentity() const;

	//! Set the translation of the current matrix. Will erase any previous values.
	CMatrix4<T> &setTranslation(const vector3d<T> &translation);

	//! Gets the current translation
	vector3d<T> getTranslation() const;

	//! Make a rotation matrix from Euler angles. The 4th row and column are unmodified.
	//! NOTE: Rotation order is ZYX. This means that vectors are
	//! first rotated around the X, then the Y, and finally the Z axis.
	//! NOTE: The rotation is done as per the right-hand rule.
	//! See test_irr_matrix4.cpp if you're still unsure about the conventions used here.
	inline CMatrix4<T> &setRotationRadians(const vector3d<T> &rotation);

	//! Same as `setRotationRadians`, but uses degrees.
	CMatrix4<T> &setRotationDegrees(const vector3d<T> &rotation);

	//! Get the rotation, as set by setRotation() when you already know the scale used to create the matrix
	/**
	NOTE: No scale value can be 0 or the result is undefined.
	NOTE: It does not necessarily return the *same* Euler angles as those set by setRotationDegrees(),
		but the rotation will be equivalent,  i.e. will have the same result when used to rotate a vector or node.
	NOTE: It will (usually) give wrong results when further transformations have been added in the matrix (like shear).
	WARNING: There have been troubles with this function over the years and we may still have missed some corner cases.
		It's generally safer to keep the rotation and scale you used to create the matrix around and work with those.
	*/
	vector3d<T> getRotationRadians(const vector3d<T> &scale) const;

	//! Returns the rotation, as set by setRotation().
	/** NOTE: You will have the same end-rotation as used in setRotation, but it might not use the same axis values.
		NOTE: This only works correctly for TRS matrix products where S is a positive, component-wise scaling (see setScale).
		NOTE: It does not necessarily return the *same* Euler angles as those set by setRotationDegrees(),
		but the rotation will be equivalent, i.e. will have the same result when used to rotate a vector or node.
	*/
	vector3d<T> getRotationRadians() const;

	//! Same as getRotationRadians, but returns degrees.
	vector3d<T> getRotationDegrees() const;

	//! Make a rotation matrix from angle and axis, assuming left handed rotation.
	/** The 4th row and column are unmodified. */
	inline CMatrix4<T> &setRotationAxisRadians(const T &angle, const vector3d<T> &axis);

	//! Set Scale
	CMatrix4<T> &setScale(const vector3d<T> &scale);

	//! Set Scale
	CMatrix4<T> &setScale(const T scale) { return setScale(vector3d<T>(scale, scale, scale)); }

	//! Get Scale
	vector3d<T> getScale() const;

	//! Translate a vector by the inverse of the translation part of this matrix.
	void inverseTranslateVect(vector3df &vect) const;

	//! Scale a vector, then rotate by the inverse of the rotation part of this matrix.
	[[nodiscard]] vector3d<T> scaleThenInvRotVect(const vector3d<T> &vect) const;

	//! Rotate and scale a vector. Applies both rotation & scale part of the matrix.
	[[nodiscard]] vector3d<T> rotateAndScaleVect(const vector3d<T> &vect) const;

	//! Transforms the vector by this matrix
	/** This operation is performed as if the vector was 4d with the 4th component = 1 */
	[[nodiscard]] vector3d<T> transformVect(const vector3d<T> &v) const;

	//! Transforms the vector by this matrix
	/** This operation is performed as if the vector was 4d with the 4th component = 1 */
	void transformVect(vector3d<T> &vect) const {
		const vector3d<T> &v = vect;
		vect = transformVect(v);
	}

	//! Transforms input vector by this matrix and stores result in output vector
	/** This operation is performed as if the vector was 4d with the 4th component = 1 */
	void transformVect(vector3d<T> &out, const vector3d<T> &in) const {
		out = transformVect(in);
	}

	//! An alternate transform vector method, writing into an array of 4 floats
	/** This operation is performed as if the vector was 4d with the 4th component =1.
		NOTE: out[3] will be written to (4th vector component)*/
	void transformVect(T *out, const vector3df &in) const;

	//! An alternate transform vector method, reading from and writing to an array of 3 floats
	/** This operation is performed as if the vector was 4d with the 4th component =1
		NOTE: out[3] will be written to (4th vector component)*/
	void transformVec3(T *out, const T *in) const;

	//! An alternate transform vector method, reading from and writing to an array of 4 floats
	void transformVec4(T *out, const T *in) const;

	//! Translate a vector by the translation part of this matrix.
	/** This operation is performed as if the vector was 4d with the 4th component =1 */
	void translateVect(vector3df &vect) const;

	//! Transforms a plane by this matrix
	void transformPlane(plane3d<f32> &plane) const;

	//! Transforms a plane by this matrix
	void transformPlane(const plane3d<f32> &in, plane3d<f32> &out) const;

	//! Transforms a axis aligned bounding box
	void transformBoxEx(aabbox3d<f32> &box) const;

	//! Multiplies this matrix by a 1x4 matrix
	void multiplyWith1x4Matrix(T *matrix) const;

	//! Calculates inverse of matrix. Slow.
	/** \return Returns false if there is no inverse matrix.*/
	bool makeInverse();

	//! Inverts a primitive matrix which only contains a translation and a rotation
	/** \param out: where result matrix is written to. */
	bool getInversePrimitive(CMatrix4<T> &out) const;

	//! Gets the inverse matrix of this one
	/** \param out: where result matrix is written to.
	\return Returns false if there is no inverse matrix. */
	bool getInverse(CMatrix4<T> &out) const;

	//! Builds a right-handed perspective projection matrix based on a field of view
	//\param zClipFromZero: Clipping of z can be projected from 0 to w when true (D3D style) and from -w to w when false (OGL style).
	CMatrix4<T> &buildProjectionMatrixPerspectiveFovRH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar, bool zClipFromZero = true);

	//! Builds a left-handed perspective projection matrix based on a field of view
	CMatrix4<T> &buildProjectionMatrixPerspectiveFovLH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar, bool zClipFromZero = true);

	//! Builds a left-handed perspective projection matrix based on a field of view, with far plane at infinity
	CMatrix4<T> &buildProjectionMatrixPerspectiveFovInfinityLH(f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 epsilon = 0);

	//! Builds a right-handed perspective projection matrix.
	CMatrix4<T> &buildProjectionMatrixPerspectiveRH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero = true);

	//! Builds a left-handed perspective projection matrix.
	CMatrix4<T> &buildProjectionMatrixPerspectiveLH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero = true);

	//! Builds a left-handed orthogonal projection matrix.
	//\param zClipFromZero: Clipping of z can be projected from 0 to 1 when true (D3D style) and from -1 to 1 when false (OGL style).
	CMatrix4<T> &buildProjectionMatrixOrthoLH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero = true);

	//! Builds a right-handed orthogonal projection matrix.
	CMatrix4<T> &buildProjectionMatrixOrthoRH(f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero = true);

	//! Builds a left-handed look-at matrix.
	CMatrix4<T> &buildCameraLookAtMatrixLH(
			const vector3df &position,
			const vector3df &target,
			const vector3df &upVector);

	//! Builds a right-handed look-at matrix.
	CMatrix4<T> &buildCameraLookAtMatrixRH(
			const vector3df &position,
			const vector3df &target,
			const vector3df &upVector);

	//! Builds a matrix that flattens geometry into a plane.
	/** \param light: light source
	\param plane: plane into which the geometry if flattened into
	\param point: value between 0 and 1, describing the light source.
	If this is 1, it is a point light, if it is 0, it is a directional light. */
	CMatrix4<T> &buildShadowMatrix(const vector3df &light, plane3df plane, f32 point = 1.0f);

	//! Builds a matrix which transforms a normalized Device Coordinate to Device Coordinates.
	/** Used to scale <-1,-1><1,1> to viewport, for example from <-1,-1> <1,1> to the viewport <0,0><0,640> */
	CMatrix4<T> &buildNDCToDCMatrix(const rect<s32> &area, f32 zScale);

	//! Creates a new matrix as interpolated matrix from two other ones.
	/** \param b: other matrix to interpolate with
	\param time: Must be a value between 0 and 1. */
	CMatrix4<T> interpolate(const CMatrix4<T> &b, f32 time) const;

	//! Gets transposed matrix
	CMatrix4<T> getTransposed() const;

	//! Gets transposed matrix
	inline void getTransposed(CMatrix4<T> &dest) const;

	//! Builds a matrix that rotates from one vector to another
	/** \param from: vector to rotate from
	\param to: vector to rotate to
	 */
	CMatrix4<T> &buildRotateFromTo(const vector3df &from, const vector3df &to);

	//! Builds a combined matrix which translates to a center before rotation and translates from origin afterwards
	/** \param center Position to rotate around
	\param translate Translation applied after the rotation
	 */
	void setRotationCenter(const vector3df &center, const vector3df &translate);

	//! Builds a matrix which rotates a source vector to a look vector over an arbitrary axis
	/** \param camPos: viewer position in world coo
	\param center: object position in world-coo and rotation pivot
	\param translation: object final translation from center
	\param axis: axis to rotate about
	\param from: source vector to rotate from
	 */
	void buildAxisAlignedBillboard(const vector3df &camPos,
			const vector3df &center,
			const vector3df &translation,
			const vector3df &axis,
			const vector3df &from);

	/*
		construct 2D Texture transformations
		rotate about center, scale, and transform.
	*/
	//! Set to a texture transformation matrix with the given parameters.
	CMatrix4<T> &buildTextureTransform(f32 rotateRad,
			const vector2df &rotatecenter,
			const vector2df &translate,
			const vector2df &scale);

	//! Set texture transformation rotation
	/** Rotate about z axis, recenter at (0.5,0.5).
	Doesn't clear other elements than those affected
	\param radAngle Angle in radians
	\return Altered matrix */
	CMatrix4<T> &setTextureRotationCenter(f32 radAngle);

	//! Set texture transformation translation
	/** Doesn't clear other elements than those affected.
	\param x Offset on x axis
	\param y Offset on y axis
	\return Altered matrix */
	CMatrix4<T> &setTextureTranslate(f32 x, f32 y);

	//! Get texture transformation translation
	/** \param x returns offset on x axis
	\param y returns offset on y axis */
	void getTextureTranslate(f32 &x, f32 &y) const;

	//! Set texture transformation translation, using a transposed representation
	/** Doesn't clear other elements than those affected.
	\param x Offset on x axis
	\param y Offset on y axis
	\return Altered matrix */
	CMatrix4<T> &setTextureTranslateTransposed(f32 x, f32 y);

	//! Set texture transformation scale
	/** Doesn't clear other elements than those affected.
	\param sx Scale factor on x axis
	\param sy Scale factor on y axis
	\return Altered matrix. */
	CMatrix4<T> &setTextureScale(f32 sx, f32 sy);

	//! Get texture transformation scale
	/** \param sx Returns x axis scale factor
	\param sy Returns y axis scale factor */
	void getTextureScale(f32 &sx, f32 &sy) const;

	//! Set texture transformation scale, and recenter at (0.5,0.5)
	/** Doesn't clear other elements than those affected.
	\param sx Scale factor on x axis
	\param sy Scale factor on y axis
	\return Altered matrix. */
	CMatrix4<T> &setTextureScaleCenter(f32 sx, f32 sy);

	//! Sets all matrix data members at once
	CMatrix4<T> &setM(const T *data);

	//! Compare two matrices using the equal method
	bool equals(const CMatrix4<T> &other, const T tolerance = (T)ROUNDING_ERROR_f64) const;

private:
	template <bool degrees>
	vector3d<T> getRotation(const vector3d<T> &scale) const;

	//! Matrix data, stored in row-major order
	T M[16];
};

} // end namespace core

#include "matrix.tpp"
