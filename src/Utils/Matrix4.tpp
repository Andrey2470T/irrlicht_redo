namespace utils {

//! Add another matrix.
template <class T>
Matrix4<T> Matrix4<T>::operator+(const Matrix4<T> &other) const
{
	Matrix4<T> temp(EM4CONST_NOTHING);

	temp[0] = M[0] + other[0];
	temp[1] = M[1] + other[1];
	temp[2] = M[2] + other[2];
	temp[3] = M[3] + other[3];
	temp[4] = M[4] + other[4];
	temp[5] = M[5] + other[5];
	temp[6] = M[6] + other[6];
	temp[7] = M[7] + other[7];
	temp[8] = M[8] + other[8];
	temp[9] = M[9] + other[9];
	temp[10] = M[10] + other[10];
	temp[11] = M[11] + other[11];
	temp[12] = M[12] + other[12];
	temp[13] = M[13] + other[13];
	temp[14] = M[14] + other[14];
	temp[15] = M[15] + other[15];

	return temp;
}

//! Add another matrix.
template <class T>
Matrix4<T> &Matrix4<T>::operator+=(const Matrix4<T> &other)
{
	M[0] += other[0];
	M[1] += other[1];
	M[2] += other[2];
	M[3] += other[3];
	M[4] += other[4];
	M[5] += other[5];
	M[6] += other[6];
	M[7] += other[7];
	M[8] += other[8];
	M[9] += other[9];
	M[10] += other[10];
	M[11] += other[11];
	M[12] += other[12];
	M[13] += other[13];
	M[14] += other[14];
	M[15] += other[15];

	return *this;
}

//! Subtract another matrix.
template <class T>
Matrix4<T> Matrix4<T>::operator-(const Matrix4<T> &other) const
{
	Matrix4<T> temp(EM4CONST_NOTHING);

	temp[0] = M[0] - other[0];
	temp[1] = M[1] - other[1];
	temp[2] = M[2] - other[2];
	temp[3] = M[3] - other[3];
	temp[4] = M[4] - other[4];
	temp[5] = M[5] - other[5];
	temp[6] = M[6] - other[6];
	temp[7] = M[7] - other[7];
	temp[8] = M[8] - other[8];
	temp[9] = M[9] - other[9];
	temp[10] = M[10] - other[10];
	temp[11] = M[11] - other[11];
	temp[12] = M[12] - other[12];
	temp[13] = M[13] - other[13];
	temp[14] = M[14] - other[14];
	temp[15] = M[15] - other[15];

	return temp;
}

//! Subtract another matrix.
template <class T>
Matrix4<T> &Matrix4<T>::operator-=(const Matrix4<T> &other)
{
	M[0] -= other[0];
	M[1] -= other[1];
	M[2] -= other[2];
	M[3] -= other[3];
	M[4] -= other[4];
	M[5] -= other[5];
	M[6] -= other[6];
	M[7] -= other[7];
	M[8] -= other[8];
	M[9] -= other[9];
	M[10] -= other[10];
	M[11] -= other[11];
	M[12] -= other[12];
	M[13] -= other[13];
	M[14] -= other[14];
	M[15] -= other[15];

	return *this;
}

//! Multiply by scalar.
template <class T>
Matrix4<T> Matrix4<T>::operator*(const T &scalar) const
{
	Matrix4<T> temp(EM4CONST_NOTHING);

	temp[0] = M[0] * scalar;
	temp[1] = M[1] * scalar;
	temp[2] = M[2] * scalar;
	temp[3] = M[3] * scalar;
	temp[4] = M[4] * scalar;
	temp[5] = M[5] * scalar;
	temp[6] = M[6] * scalar;
	temp[7] = M[7] * scalar;
	temp[8] = M[8] * scalar;
	temp[9] = M[9] * scalar;
	temp[10] = M[10] * scalar;
	temp[11] = M[11] * scalar;
	temp[12] = M[12] * scalar;
	temp[13] = M[13] * scalar;
	temp[14] = M[14] * scalar;
	temp[15] = M[15] * scalar;

	return temp;
}

//! Multiply by scalar.
template <class T>
Matrix4<T> &Matrix4<T>::operator*=(const T &scalar)
{
	M[0] *= scalar;
	M[1] *= scalar;
	M[2] *= scalar;
	M[3] *= scalar;
	M[4] *= scalar;
	M[5] *= scalar;
	M[6] *= scalar;
	M[7] *= scalar;
	M[8] *= scalar;
	M[9] *= scalar;
	M[10] *= scalar;
	M[11] *= scalar;
	M[12] *= scalar;
	M[13] *= scalar;
	M[14] *= scalar;
	M[15] *= scalar;

	return *this;
}

//! Multiply by another matrix.
template <class T>
Matrix4<T> &Matrix4<T>::operator*=(const Matrix4<T> &other)
{
	Matrix4<T> temp(*this);
	return setbyproduct_nocheck(temp, other);
}

//! multiply by another matrix
// set this matrix to the product of two other matrices
// goal is to reduce stack use and copy
template <class T>
Matrix4<T> &Matrix4<T>::setbyproduct_nocheck(const Matrix4<T> &other_a, const Matrix4<T> &other_b)
{
	const T *m1 = other_a.M;
	const T *m2 = other_b.M;

	M[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
	M[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
	M[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
	M[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

	M[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
	M[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
	M[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
	M[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

	M[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
	M[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
	M[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
	M[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

	M[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
	M[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
	M[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	M[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

	return *this;
}

//! multiply by another matrix
// set this matrix to the product of two other matrices
// goal is to reduce stack use and copy
template <class T>
Matrix4<T> &Matrix4<T>::setbyproduct(const Matrix4<T> &other_a, const Matrix4<T> &other_b)
{
	return setbyproduct_nocheck(other_a, other_b);
}

//! multiply by another matrix
template <class T>
Matrix4<T> Matrix4<T>::operator*(const Matrix4<T> &m2) const
{
	Matrix4<T> m3(EM4CONST_NOTHING);

	const T *m1 = M;

	m3[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
	m3[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
	m3[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
	m3[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

	m3[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
	m3[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
	m3[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
	m3[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

	m3[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
	m3[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
	m3[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
	m3[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

	m3[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
	m3[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
	m3[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
	m3[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
	return m3;
}

template <class T>
Vector3D<T> Matrix4<T>::getTranslation() const
{
	return Vector3D<T>(M[12], M[13], M[14]);
}

template <class T>
Matrix4<T> &Matrix4<T>::setTranslation(const Vector3D<T> &translation)
{
	M[12] = translation.X;
	M[13] = translation.Y;
	M[14] = translation.Z;

	return *this;
}

template <class T>
Matrix4<T> &Matrix4<T>::setInverseTranslation(const Vector3D<T> &translation)
{
	M[12] = -translation.X;
	M[13] = -translation.Y;
	M[14] = -translation.Z;

	return *this;
}

template <class T>
Matrix4<T> &Matrix4<T>::setScale(const Vector3D<T> &scale)
{
	M[0] = scale.X;
	M[5] = scale.Y;
	M[10] = scale.Z;

	return *this;
}

//! Returns the absolute values of the scales of the matrix.
/**
Note: You only get back original values if the matrix only set the scale.
Otherwise the result is a scale you can use to normalize the matrix axes,
but it's usually no longer what you did set with setScale.
*/
template <class T>
Vector3D<T> Matrix4<T>::getScale() const
{
	// See http://www.robertblum.com/articles/2005/02/14/decomposing-matrices

	// Deal with the 0 rotation case first
	// Prior to Irrlicht 1.6, we always returned this value.
    if (utils::equals(M[1], 0) && utils::equals(M[2], 0) &&
            utils::equals(M[4], 0) && utils::equals(M[6], 0) &&
            utils::equals(M[8], 0) && utils::equals(M[9], 0))
		return Vector3D<T>(M[0], M[5], M[10]);

	// We have to do the full calculation.
	return Vector3D<T>(sqrtf(M[0] * M[0] + M[1] * M[1] + M[2] * M[2]),
			sqrtf(M[4] * M[4] + M[5] * M[5] + M[6] * M[6]),
			sqrtf(M[8] * M[8] + M[9] * M[9] + M[10] * M[10]));
}

template <class T>
Matrix4<T> &Matrix4<T>::setRotationDegrees(const Vector3D<T> &rotation)
{
    return setRotationRadians(rotation * DEGTORAD);
}

template <class T>
Matrix4<T> &Matrix4<T>::setInverseRotationDegrees(const Vector3D<T> &rotation)
{
    return setInverseRotationRadians(rotation * DEGTORAD);
}

template <class T>
Matrix4<T> &Matrix4<T>::setRotationRadians(const Vector3D<T> &rotation)
{
	const f64 cr = cos(rotation.X);
	const f64 sr = sin(rotation.X);
	const f64 cp = cos(rotation.Y);
	const f64 sp = sin(rotation.Y);
	const f64 cy = cos(rotation.Z);
	const f64 sy = sin(rotation.Z);

	M[0] = (T)(cp * cy);
	M[1] = (T)(cp * sy);
	M[2] = (T)(-sp);

	const f64 srsp = sr * sp;
	const f64 crsp = cr * sp;

	M[4] = (T)(srsp * cy - cr * sy);
	M[5] = (T)(srsp * sy + cr * cy);
	M[6] = (T)(sr * cp);

	M[8] = (T)(crsp * cy + sr * sy);
	M[9] = (T)(crsp * sy - sr * cy);
	M[10] = (T)(cr * cp);

	return *this;
}

//! Returns a rotation which (mostly) works in combination with the given scale
/**
This code was originally written by by Chev (assuming no scaling back then,
we can be blamed for all problems added by regarding scale)
*/
template <class T>
Vector3D<T> Matrix4<T>::getRotationDegrees(const Vector3D<T> &scale_) const
{
	const Matrix4<T> &mat = *this;
    const Vector3D<f64> scale(utils::equals(scale_.X, 0) ? T_MAX(f32) : scale_.X, utils::equals(scale_.Y, 0) ? T_MAX(f32) : scale_.Y, utils::equals(scale_.Z, 0) ? T_MAX(f32) : scale_.Z);
    const Vector3D<f64> invScale(1.0 / scale.X, 1.0 / scale.Y, 1.0 / scale.Z);

    f64 Y = -asin(std::clamp(mat[2] * invScale.X, -1.0, 1.0));
	const f64 C = cos(Y);
    Y *= radToDeg(Y);

	f64 rotx, roty, X, Z;

    if (!utils::equals((T)C, 0)) {
        const f64 invC = 1.0 / C;
		rotx = mat[10] * invC * invScale.Z;
		roty = mat[6] * invC * invScale.Y;
        X = radToDeg(atan2(roty, rotx));
		rotx = mat[0] * invC * invScale.X;
		roty = mat[1] * invC * invScale.X;
        Z = radToDeg(atan2(roty, rotx));
	} else {
		X = 0.0;
		rotx = mat[5] * invScale.Y;
		roty = -mat[4] * invScale.Y;
        Z = radToDeg(atan2(roty, rotx));
	}

	// fix values that get below zero
	if (X < 0.0)
		X += 360.0;
	if (Y < 0.0)
		Y += 360.0;
	if (Z < 0.0)
		Z += 360.0;

	return Vector3D<T>((T)X, (T)Y, (T)Z);
}

//! Returns a rotation that is equivalent to that set by setRotationDegrees().
template <class T>
Vector3D<T> Matrix4<T>::getRotationDegrees() const
{
	// Note: Using getScale() here make it look like it could do matrix decomposition.
	// It can't! It works (or should work) as long as rotation doesn't flip the handedness
	// aka scale swapping 1 or 3 axes. (I think we could catch that as well by comparing
	// crossproduct of first 2 axes to direction of third axis, but TODO)
	// And maybe it should also offer the solution for the simple calculation
	// without regarding scaling as Irrlicht did before 1.7
	Vector3D<T> scale(getScale());

	// We assume the matrix uses rotations instead of negative scaling 2 axes.
	// Otherwise it fails even for some simple cases, like rotating around
	// 2 axes by 180° which getScale thinks is a negative scaling.
	if (scale.Y < 0 && scale.Z < 0) {
		scale.Y = -scale.Y;
		scale.Z = -scale.Z;
	} else if (scale.X < 0 && scale.Z < 0) {
		scale.X = -scale.X;
		scale.Z = -scale.Z;
	} else if (scale.X < 0 && scale.Y < 0) {
		scale.X = -scale.X;
		scale.Y = -scale.Y;
	}

	return getRotationDegrees(scale);
}

//! Sets matrix to rotation matrix of inverse angles given as parameters
template <class T>
Matrix4<T> &Matrix4<T>::setInverseRotationRadians(const Vector3D<T> &rotation)
{
	f64 cr = cos(rotation.X);
	f64 sr = sin(rotation.X);
	f64 cp = cos(rotation.Y);
	f64 sp = sin(rotation.Y);
	f64 cy = cos(rotation.Z);
	f64 sy = sin(rotation.Z);

	M[0] = (T)(cp * cy);
	M[4] = (T)(cp * sy);
	M[8] = (T)(-sp);

	f64 srsp = sr * sp;
	f64 crsp = cr * sp;

	M[1] = (T)(srsp * cy - cr * sy);
	M[5] = (T)(srsp * sy + cr * cy);
	M[9] = (T)(sr * cp);

	M[2] = (T)(crsp * cy + sr * sy);
	M[6] = (T)(crsp * sy - sr * cy);
	M[10] = (T)(cr * cp);

	return *this;
}

//! Sets matrix to rotation matrix defined by axis and angle, assuming LH rotation
template <class T>
Matrix4<T> &Matrix4<T>::setRotationAxisRadians(const T &angle, const Vector3D<T> &axis)
{
	const f64 c = cos(angle);
	const f64 s = sin(angle);
	const f64 t = 1.0 - c;

	const f64 tx = t * axis.X;
	const f64 ty = t * axis.Y;
	const f64 tz = t * axis.Z;

	const f64 sx = s * axis.X;
	const f64 sy = s * axis.Y;
	const f64 sz = s * axis.Z;

	M[0] = (T)(tx * axis.X + c);
	M[1] = (T)(tx * axis.Y + sz);
	M[2] = (T)(tx * axis.Z - sy);

	M[4] = (T)(ty * axis.X - sz);
	M[5] = (T)(ty * axis.Y + c);
	M[6] = (T)(ty * axis.Z + sx);

	M[8] = (T)(tz * axis.X + sy);
	M[9] = (T)(tz * axis.Y - sx);
	M[10] = (T)(tz * axis.Z + c);

	return *this;
}

/*!
 */
template <class T>
Matrix4<T> &Matrix4<T>::makeNull()
{
    memset(M, 0, 16 * sizeof(T));;

    return *this;
}

/*!
 */
template <class T>
Matrix4<T> &Matrix4<T>::makeIdentity()
{
	memset(M, 0, 16 * sizeof(T));
	M[0] = M[5] = M[10] = M[15] = (T)1;

	return *this;
}

/*
	check identity with epsilon
	solve floating range problems..
*/
template <class T>
bool Matrix4<T>::isIdentity() const
{
	if (!utils::equals(M[12], (T)0) || !utils::equals(M[13], (T)0) || !utils::equals(M[14], (T)0) || !utils::equals(M[15], (T)1))
		return false;

	if (!utils::equals(M[0], (T)1) || !utils::equals(M[1], (T)0) || !utils::equals(M[2], (T)0) || !utils::equals(M[3], (T)0))
		return false;

	if (!utils::equals(M[4], (T)0) || !utils::equals(M[5], (T)1) || !utils::equals(M[6], (T)0) || !utils::equals(M[7], (T)0))
		return false;

	if (!utils::equals(M[8], (T)0) || !utils::equals(M[9], (T)0) || !utils::equals(M[10], (T)1) || !utils::equals(M[11], (T)0))
		return false;
/*
		if (!utils::equals( M[ 0], (T)1 ) ||
			!utils::equals( M[ 5], (T)1 ) ||
			!utils::equals( M[10], (T)1 ) ||
			!utils::equals( M[15], (T)1 ))
			return false;

		for (s32 i=0; i<4; ++i)
			for (s32 j=0; j<4; ++j)
				if ((j != i) && (!iszero((*this)(i,j))))
					return false;
*/

	return true;
}

/* Check orthogonality of matrix. */
template <class T>
bool Matrix4<T>::isOrthogonal() const
{
	T dp = M[0] * M[4] + M[1] * M[5] + M[2] * M[6] + M[3] * M[7];
	if (!iszero(dp))
		return false;
	dp = M[0] * M[8] + M[1] * M[9] + M[2] * M[10] + M[3] * M[11];
	if (!iszero(dp))
		return false;
	dp = M[0] * M[12] + M[1] * M[13] + M[2] * M[14] + M[3] * M[15];
	if (!iszero(dp))
		return false;
	dp = M[4] * M[8] + M[5] * M[9] + M[6] * M[10] + M[7] * M[11];
	if (!iszero(dp))
		return false;
	dp = M[4] * M[12] + M[5] * M[13] + M[6] * M[14] + M[7] * M[15];
	if (!iszero(dp))
		return false;
	dp = M[8] * M[12] + M[9] * M[13] + M[10] * M[14] + M[11] * M[15];
	return (iszero(dp));
}

template <class T>
Vector3D<T> Matrix4<T>::rotateAndScaleVect(const Vector3D<T> &v) const
{
	return {
		v.X * M[0] + v.Y * M[4] + v.Z * M[8],
		v.X * M[1] + v.Y * M[5] + v.Z * M[9],
		v.X * M[2] + v.Y * M[6] + v.Z * M[10]
	};
}

template <class T>
Vector3D<T> Matrix4<T>::scaleThenInvRotVect(const Vector3D<T> &v) const
{
	return {
		v.X * M[0] + v.Y * M[1] + v.Z * M[2],
		v.X * M[4] + v.Y * M[5] + v.Z * M[6],
		v.X * M[8] + v.Y * M[9] + v.Z * M[10]
	};
}

template <class T>
v3f &Matrix4<T>::transformVect(v3f &vect) const
{
	T vector[3];

	vector[0] = vect.X * M[0] + vect.Y * M[4] + vect.Z * M[8] + M[12];
	vector[1] = vect.X * M[1] + vect.Y * M[5] + vect.Z * M[9] + M[13];
	vector[2] = vect.X * M[2] + vect.Y * M[6] + vect.Z * M[10] + M[14];

	vect.X = static_cast<f32>(vector[0]);
	vect.Y = static_cast<f32>(vector[1]);
	vect.Z = static_cast<f32>(vector[2]);

    return vect;
}

template <class T>
void Matrix4<T>::transformVect(v3f &out, const v3f &in) const
{
	out.X = in.X * M[0] + in.Y * M[4] + in.Z * M[8] + M[12];
	out.Y = in.X * M[1] + in.Y * M[5] + in.Z * M[9] + M[13];
	out.Z = in.X * M[2] + in.Y * M[6] + in.Z * M[10] + M[14];
}

template <class T>
void Matrix4<T>::transformVect(T *out, const v3f &in) const
{
	out[0] = in.X * M[0] + in.Y * M[4] + in.Z * M[8] + M[12];
	out[1] = in.X * M[1] + in.Y * M[5] + in.Z * M[9] + M[13];
	out[2] = in.X * M[2] + in.Y * M[6] + in.Z * M[10] + M[14];
	out[3] = in.X * M[3] + in.Y * M[7] + in.Z * M[11] + M[15];
}

template <class T>
void Matrix4<T>::transformVec3(T *out, const T *in) const
{
	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8] + M[12];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9] + M[13];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10] + M[14];
}

template <class T>
void Matrix4<T>::transformVec4(T *out, const T *in) const
{
	out[0] = in[0] * M[0] + in[1] * M[4] + in[2] * M[8] + in[3] * M[12];
	out[1] = in[0] * M[1] + in[1] * M[5] + in[2] * M[9] + in[3] * M[13];
	out[2] = in[0] * M[2] + in[1] * M[6] + in[2] * M[10] + in[3] * M[14];
	out[3] = in[0] * M[3] + in[1] * M[7] + in[2] * M[11] + in[3] * M[15];
}

//! Transforms a plane by this matrix
template <class T>
void Matrix4<T>::transformPlane(plane3f &plane) const
{
	v3f member;
	// Transform the plane member point, i.e. rotate, translate and scale it.
	transformVect(member, plane.getMemberPoint());

	// Transform the normal by the transposed inverse of the matrix
	Matrix4<T> transposedInverse(*this, EM4CONST_INVERSE_TRANSPOSED);
	v3f normal = transposedInverse.rotateAndScaleVect(plane.Normal);
	plane.setPlane(member, normal.normalize());
}

//! Transforms a plane by this matrix
template <class T>
void Matrix4<T>::transformPlane(const plane3f &in, utils::plane3f &out) const
{
	out = in;
	transformPlane(out);
}

//! Transforms a axis aligned bounding box more accurately than transformBox()
template <class T>
void Matrix4<T>::transformBoxEx(aabbf &box) const
{
	const f32 Amin[3] = {box.MinEdge.X, box.MinEdge.Y, box.MinEdge.Z};
	const f32 Amax[3] = {box.MaxEdge.X, box.MaxEdge.Y, box.MaxEdge.Z};

	f32 Bmin[3];
	f32 Bmax[3];

	Bmin[0] = Bmax[0] = M[12];
	Bmin[1] = Bmax[1] = M[13];
	Bmin[2] = Bmax[2] = M[14];

	const Matrix4<T> &m = *this;

	for (u32 i = 0; i < 3; ++i) {
		for (u32 j = 0; j < 3; ++j) {
			const f32 a = m(j, i) * Amin[j];
			const f32 b = m(j, i) * Amax[j];

			if (a < b) {
				Bmin[i] += a;
				Bmax[i] += b;
			} else {
				Bmin[i] += b;
				Bmax[i] += a;
			}
		}
	}

	box.MinEdge.X = Bmin[0];
	box.MinEdge.Y = Bmin[1];
	box.MinEdge.Z = Bmin[2];

	box.MaxEdge.X = Bmax[0];
	box.MaxEdge.Y = Bmax[1];
	box.MaxEdge.Z = Bmax[2];
}

//! Multiplies this matrix by a 1x4 matrix
template <class T>
void Matrix4<T>::multiplyWith1x4Matrix(T *matrix) const
{
	/*
	0  1  2  3
	4  5  6  7
	8  9  10 11
	12 13 14 15
	*/

	T mat[4];
	mat[0] = matrix[0];
	mat[1] = matrix[1];
	mat[2] = matrix[2];
	mat[3] = matrix[3];

	matrix[0] = M[0] * mat[0] + M[4] * mat[1] + M[8] * mat[2] + M[12] * mat[3];
	matrix[1] = M[1] * mat[0] + M[5] * mat[1] + M[9] * mat[2] + M[13] * mat[3];
	matrix[2] = M[2] * mat[0] + M[6] * mat[1] + M[10] * mat[2] + M[14] * mat[3];
	matrix[3] = M[3] * mat[0] + M[7] * mat[1] + M[11] * mat[2] + M[15] * mat[3];
}

template <class T>
void Matrix4<T>::inverseTranslateVect(v3f &vect) const
{
	vect.X = vect.X - M[12];
	vect.Y = vect.Y - M[13];
	vect.Z = vect.Z - M[14];
}

template <class T>
void Matrix4<T>::translateVect(v3f &vect) const
{
	vect.X = vect.X + M[12];
	vect.Y = vect.Y + M[13];
	vect.Z = vect.Z + M[14];
}

template <class T>
bool Matrix4<T>::getInverse(Matrix4<T> &out) const
{
	/// Calculates the inverse of this Matrix
	/// The inverse is calculated using Cramers rule.
	/// If no inverse exists then 'false' is returned.

	const Matrix4<T> &m = *this;

	f32 d = (m[0] * m[5] - m[1] * m[4]) * (m[10] * m[15] - m[11] * m[14]) -
			(m[0] * m[6] - m[2] * m[4]) * (m[9] * m[15] - m[11] * m[13]) +
			(m[0] * m[7] - m[3] * m[4]) * (m[9] * m[14] - m[10] * m[13]) +
			(m[1] * m[6] - m[2] * m[5]) * (m[8] * m[15] - m[11] * m[12]) -
			(m[1] * m[7] - m[3] * m[5]) * (m[8] * m[14] - m[10] * m[12]) +
			(m[2] * m[7] - m[3] * m[6]) * (m[8] * m[13] - m[9] * m[12]);

    if (utils::equals(d, 0, T_MIN(f32)))
		return false;

    d = 1.0 / d;

	out[0] = d * (m[5] * (m[10] * m[15] - m[11] * m[14]) +
						 m[6] * (m[11] * m[13] - m[9] * m[15]) +
						 m[7] * (m[9] * m[14] - m[10] * m[13]));
	out[1] = d * (m[9] * (m[2] * m[15] - m[3] * m[14]) +
						 m[10] * (m[3] * m[13] - m[1] * m[15]) +
						 m[11] * (m[1] * m[14] - m[2] * m[13]));
	out[2] = d * (m[13] * (m[2] * m[7] - m[3] * m[6]) +
						 m[14] * (m[3] * m[5] - m[1] * m[7]) +
						 m[15] * (m[1] * m[6] - m[2] * m[5]));
	out[3] = d * (m[1] * (m[7] * m[10] - m[6] * m[11]) +
						 m[2] * (m[5] * m[11] - m[7] * m[9]) +
						 m[3] * (m[6] * m[9] - m[5] * m[10]));
	out[4] = d * (m[6] * (m[8] * m[15] - m[11] * m[12]) +
						 m[7] * (m[10] * m[12] - m[8] * m[14]) +
						 m[4] * (m[11] * m[14] - m[10] * m[15]));
	out[5] = d * (m[10] * (m[0] * m[15] - m[3] * m[12]) +
						 m[11] * (m[2] * m[12] - m[0] * m[14]) +
						 m[8] * (m[3] * m[14] - m[2] * m[15]));
	out[6] = d * (m[14] * (m[0] * m[7] - m[3] * m[4]) +
						 m[15] * (m[2] * m[4] - m[0] * m[6]) +
						 m[12] * (m[3] * m[6] - m[2] * m[7]));
	out[7] = d * (m[2] * (m[7] * m[8] - m[4] * m[11]) +
						 m[3] * (m[4] * m[10] - m[6] * m[8]) +
						 m[0] * (m[6] * m[11] - m[7] * m[10]));
	out[8] = d * (m[7] * (m[8] * m[13] - m[9] * m[12]) +
						 m[4] * (m[9] * m[15] - m[11] * m[13]) +
						 m[5] * (m[11] * m[12] - m[8] * m[15]));
	out[9] = d * (m[11] * (m[0] * m[13] - m[1] * m[12]) +
						 m[8] * (m[1] * m[15] - m[3] * m[13]) +
						 m[9] * (m[3] * m[12] - m[0] * m[15]));
	out[10] = d * (m[15] * (m[0] * m[5] - m[1] * m[4]) +
						  m[12] * (m[1] * m[7] - m[3] * m[5]) +
						  m[13] * (m[3] * m[4] - m[0] * m[7]));
	out[11] = d * (m[3] * (m[5] * m[8] - m[4] * m[9]) +
						  m[0] * (m[7] * m[9] - m[5] * m[11]) +
						  m[1] * (m[4] * m[11] - m[7] * m[8]));
	out[12] = d * (m[4] * (m[10] * m[13] - m[9] * m[14]) +
						  m[5] * (m[8] * m[14] - m[10] * m[12]) +
						  m[6] * (m[9] * m[12] - m[8] * m[13]));
	out[13] = d * (m[8] * (m[2] * m[13] - m[1] * m[14]) +
						  m[9] * (m[0] * m[14] - m[2] * m[12]) +
						  m[10] * (m[1] * m[12] - m[0] * m[13]));
	out[14] = d * (m[12] * (m[2] * m[5] - m[1] * m[6]) +
						  m[13] * (m[0] * m[6] - m[2] * m[4]) +
						  m[14] * (m[1] * m[4] - m[0] * m[5]));
	out[15] = d * (m[0] * (m[5] * m[10] - m[6] * m[9]) +
						  m[1] * (m[6] * m[8] - m[4] * m[10]) +
						  m[2] * (m[4] * m[9] - m[5] * m[8]));

	return true;
}

//! Inverts a primitive matrix which only contains a translation and a rotation
//! \param out: where result matrix is written to.
template <class T>
bool Matrix4<T>::getInversePrimitive(Matrix4<T> &out) const
{
	out.M[0] = M[0];
	out.M[1] = M[4];
	out.M[2] = M[8];
	out.M[3] = 0;

	out.M[4] = M[1];
	out.M[5] = M[5];
	out.M[6] = M[9];
	out.M[7] = 0;

	out.M[8] = M[2];
	out.M[9] = M[6];
	out.M[10] = M[10];
	out.M[11] = 0;

	out.M[12] = (T) - (M[12] * M[0] + M[13] * M[1] + M[14] * M[2]);
	out.M[13] = (T) - (M[12] * M[4] + M[13] * M[5] + M[14] * M[6]);
	out.M[14] = (T) - (M[12] * M[8] + M[13] * M[9] + M[14] * M[10]);
	out.M[15] = 1;

	return true;
}

/*!
 */
template <class T>
bool Matrix4<T>::makeInverse()
{
	Matrix4<T> temp(EM4CONST_NOTHING);

	if (getInverse(temp)) {
		*this = temp;
		return true;
	}

	return false;
}

template <class T>
Matrix4<T> &Matrix4<T>::operator=(const T &scalar)
{
	for (s32 i = 0; i < 16; ++i)
		M[i] = scalar;

	return *this;
}

// Builds a right-handed perspective projection matrix based on a field of view
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixPerspectiveFovRH(
		f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar, bool zClipFromZero)
{
    const f64 h = 1.0 / (tan(fieldOfViewRadians * 0.5));
    assert(aspectRatio != 0.f); // divide by zero
	const T w = static_cast<T>(h / aspectRatio);

    assert(zNear != zFar); // divide by zero
	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	// M[10]
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	// M[14]
	M[15] = 0;

	if (zClipFromZero) { // DirectX version
		M[10] = (T)(zFar / (zNear - zFar));
		M[14] = (T)(zNear * zFar / (zNear - zFar));
	} else // OpenGL version
	{
		M[10] = (T)((zFar + zNear) / (zNear - zFar));
		M[14] = (T)(2.0f * zNear * zFar / (zNear - zFar));
	}

	return *this;
}

// Builds a left-handed perspective projection matrix based on a field of view
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixPerspectiveFovLH(
		f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 zFar, bool zClipFromZero)
{
    const f64 h = 1.0 / (tan(fieldOfViewRadians * 0.5));
    assert(aspectRatio != 0.f); // divide by zero
	const T w = static_cast<T>(h / aspectRatio);

    assert(zNear != zFar); // divide by zero
	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	// M[10]
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	// M[14]
	M[15] = 0;

	if (zClipFromZero) { // DirectX version
		M[10] = (T)(zFar / (zFar - zNear));
		M[14] = (T)(-zNear * zFar / (zFar - zNear));
	} else // OpenGL version
	{
		M[10] = (T)((zFar + zNear) / (zFar - zNear));
		M[14] = (T)(2.0f * zNear * zFar / (zNear - zFar));
	}

	return *this;
}

// Builds a left-handed perspective projection matrix based on a field of view, with far plane culling at infinity
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixPerspectiveFovInfinityLH(
		f32 fieldOfViewRadians, f32 aspectRatio, f32 zNear, f32 epsilon)
{
    const f64 h = 1.0 / (tan(fieldOfViewRadians * 0.5));
    assert(aspectRatio != 0.f); // divide by zero
	const T w = static_cast<T>(h / aspectRatio);

	M[0] = w;
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)h;
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	M[10] = (T)(1.f - epsilon);
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	M[14] = (T)(zNear * (epsilon - 1.f));
	M[15] = 0;

	return *this;
}

// Builds a left-handed orthogonal projection matrix.
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixOrthoLH(
		f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero)
{
    assert(widthOfViewVolume != 0.f);  // divide by zero
    assert(heightOfViewVolume != 0.f); // divide by zero
    assert(zNear != zFar);             // divide by zero
	M[0] = (T)(2 / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	// M[10]
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	// M[14]
	M[15] = 1;

	if (zClipFromZero) {
		M[10] = (T)(1 / (zFar - zNear));
		M[14] = (T)(zNear / (zNear - zFar));
	} else {
		M[10] = (T)(2 / (zFar - zNear));
		M[14] = (T) - (zFar + zNear) / (zFar - zNear);
	}

	return *this;
}

// Builds a right-handed orthogonal projection matrix.
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixOrthoRH(
		f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero)
{
    assert(widthOfViewVolume != 0.f);  // divide by zero
    assert(heightOfViewVolume != 0.f); // divide by zero
    assert(zNear != zFar);             // divide by zero
	M[0] = (T)(2 / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	// M[10]
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	// M[14]
	M[15] = 1;

	if (zClipFromZero) {
		M[10] = (T)(1 / (zNear - zFar));
		M[14] = (T)(zNear / (zNear - zFar));
	} else {
		M[10] = (T)(2 / (zNear - zFar));
		M[14] = (T) - (zFar + zNear) / (zFar - zNear);
	}

	return *this;
}

// Builds a right-handed perspective projection matrix.
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixPerspectiveRH(
		f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero)
{
    assert(widthOfViewVolume != 0.f);  // divide by zero
    assert(heightOfViewVolume != 0.f); // divide by zero
    assert(zNear != zFar);             // divide by zero
	M[0] = (T)(2 * zNear / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 * zNear / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	// M[10]
	M[11] = -1;

	M[12] = 0;
	M[13] = 0;
	// M[14]
	M[15] = 0;

	if (zClipFromZero) { // DirectX version
		M[10] = (T)(zFar / (zNear - zFar));
		M[14] = (T)(zNear * zFar / (zNear - zFar));
	} else // OpenGL version
	{
		M[10] = (T)((zFar + zNear) / (zNear - zFar));
		M[14] = (T)(2.0f * zNear * zFar / (zNear - zFar));
	}

	return *this;
}

// Builds a left-handed perspective projection matrix.
template <class T>
Matrix4<T> &Matrix4<T>::buildProjectionMatrixPerspectiveLH(
		f32 widthOfViewVolume, f32 heightOfViewVolume, f32 zNear, f32 zFar, bool zClipFromZero)
{
    assert(widthOfViewVolume != 0.f);  // divide by zero
    assert(heightOfViewVolume != 0.f); // divide by zero
    assert(zNear != zFar);             // divide by zero
	M[0] = (T)(2 * zNear / widthOfViewVolume);
	M[1] = 0;
	M[2] = 0;
	M[3] = 0;

	M[4] = 0;
	M[5] = (T)(2 * zNear / heightOfViewVolume);
	M[6] = 0;
	M[7] = 0;

	M[8] = 0;
	M[9] = 0;
	// M[10]
	M[11] = 1;

	M[12] = 0;
	M[13] = 0;
	// M[14] = (T)(zNear*zFar/(zNear-zFar));
	M[15] = 0;

	if (zClipFromZero) { // DirectX version
		M[10] = (T)(zFar / (zFar - zNear));
		M[14] = (T)(zNear * zFar / (zNear - zFar));
	} else // OpenGL version
	{
		M[10] = (T)((zFar + zNear) / (zFar - zNear));
		M[14] = (T)(2.0f * zNear * zFar / (zNear - zFar));
	}

	return *this;
}

// Builds a matrix that flattens geometry into a plane.
template <class T>
Matrix4<T> &Matrix4<T>::buildShadowMatrix(const v3f &light, utils::plane3f plane, f32 point)
{
	plane.Normal.normalize();
	const f32 d = plane.Normal.dotProduct(light);

	M[0] = (T)(-plane.Normal.X * light.X + d);
	M[1] = (T)(-plane.Normal.X * light.Y);
	M[2] = (T)(-plane.Normal.X * light.Z);
	M[3] = (T)(-plane.Normal.X * point);

	M[4] = (T)(-plane.Normal.Y * light.X);
	M[5] = (T)(-plane.Normal.Y * light.Y + d);
	M[6] = (T)(-plane.Normal.Y * light.Z);
	M[7] = (T)(-plane.Normal.Y * point);

	M[8] = (T)(-plane.Normal.Z * light.X);
	M[9] = (T)(-plane.Normal.Z * light.Y);
	M[10] = (T)(-plane.Normal.Z * light.Z + d);
	M[11] = (T)(-plane.Normal.Z * point);

	M[12] = (T)(-plane.D * light.X);
	M[13] = (T)(-plane.D * light.Y);
	M[14] = (T)(-plane.D * light.Z);
	M[15] = (T)(-plane.D * point + d);

	return *this;
}

// Builds a left-handed look-at matrix.
template <class T>
Matrix4<T> &Matrix4<T>::buildCameraLookAtMatrixLH(
		const v3f &position,
		const v3f &target,
		const v3f &upVector)
{
	v3f zaxis = target - position;
	zaxis.normalize();

	v3f xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	v3f yaxis = zaxis.crossProduct(xaxis);

	M[0] = (T)xaxis.X;
	M[1] = (T)yaxis.X;
	M[2] = (T)zaxis.X;
	M[3] = 0;

	M[4] = (T)xaxis.Y;
	M[5] = (T)yaxis.Y;
	M[6] = (T)zaxis.Y;
	M[7] = 0;

	M[8] = (T)xaxis.Z;
	M[9] = (T)yaxis.Z;
	M[10] = (T)zaxis.Z;
	M[11] = 0;

	M[12] = (T)-xaxis.dotProduct(position);
	M[13] = (T)-yaxis.dotProduct(position);
	M[14] = (T)-zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}

// Builds a right-handed look-at matrix.
template <class T>
Matrix4<T> &Matrix4<T>::buildCameraLookAtMatrixRH(
		const v3f &position,
		const v3f &target,
		const v3f &upVector)
{
	v3f zaxis = position - target;
	zaxis.normalize();

	v3f xaxis = upVector.crossProduct(zaxis);
	xaxis.normalize();

	v3f yaxis = zaxis.crossProduct(xaxis);

	M[0] = (T)xaxis.X;
	M[1] = (T)yaxis.X;
	M[2] = (T)zaxis.X;
	M[3] = 0;

	M[4] = (T)xaxis.Y;
	M[5] = (T)yaxis.Y;
	M[6] = (T)zaxis.Y;
	M[7] = 0;

	M[8] = (T)xaxis.Z;
	M[9] = (T)yaxis.Z;
	M[10] = (T)zaxis.Z;
	M[11] = 0;

	M[12] = (T)-xaxis.dotProduct(position);
	M[13] = (T)-yaxis.dotProduct(position);
	M[14] = (T)-zaxis.dotProduct(position);
	M[15] = 1;

	return *this;
}

// creates a new matrix as interpolated matrix from this and the passed one.
template <class T>
Matrix4<T> Matrix4<T>::interpolate(const utils::Matrix4<T> &b, f32 time) const
{
	Matrix4<T> mat(EM4CONST_NOTHING);

	for (u32 i = 0; i < 16; i += 4) {
		mat.M[i + 0] = (T)(M[i + 0] + (b.M[i + 0] - M[i + 0]) * time);
		mat.M[i + 1] = (T)(M[i + 1] + (b.M[i + 1] - M[i + 1]) * time);
		mat.M[i + 2] = (T)(M[i + 2] + (b.M[i + 2] - M[i + 2]) * time);
		mat.M[i + 3] = (T)(M[i + 3] + (b.M[i + 3] - M[i + 3]) * time);
	}
	return mat;
}

// returns transposed matrix
template <class T>
Matrix4<T> Matrix4<T>::getTransposed() const
{
	Matrix4<T> t(EM4CONST_NOTHING);
	getTransposed(t);
	return t;
}

// returns transposed matrix
template <class T>
void Matrix4<T>::getTransposed(Matrix4<T> &o) const
{
	o[0] = M[0];
	o[1] = M[4];
	o[2] = M[8];
	o[3] = M[12];

	o[4] = M[1];
	o[5] = M[5];
	o[6] = M[9];
	o[7] = M[13];

	o[8] = M[2];
	o[9] = M[6];
	o[10] = M[10];
	o[11] = M[14];

	o[12] = M[3];
	o[13] = M[7];
	o[14] = M[11];
	o[15] = M[15];
}

// used to scale <-1,-1><1,1> to viewport
template <class T>
Matrix4<T> &Matrix4<T>::buildNDCToDCMatrix(recti &viewport, f32 zScale)
{
	const f32 scaleX = (viewport.getWidth() - 0.75f) * 0.5f;
	const f32 scaleY = -(viewport.getHeight() - 0.75f) * 0.5f;

	const f32 dx = -0.5f + ((viewport.ULC.X + viewport.LRC.X) * 0.5f);
	const f32 dy = -0.5f + ((viewport.ULC.Y + viewport.LRC.Y) * 0.5f);

	makeIdentity();
	M[12] = (T)dx;
	M[13] = (T)dy;
	return setScale(Vector3D<T>((T)scaleX, (T)scaleY, (T)zScale));
}

//! Builds a matrix that rotates from one vector to another
/** \param from: vector to rotate from
\param to: vector to rotate to

	http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToMatrix/index.htm
 */
template <class T>
Matrix4<T> &Matrix4<T>::buildRotateFromTo(const v3f &from, const v3f &to)
{
	// unit vectors
	v3f f(from);
	v3f t(to);
	f.normalize();
	t.normalize();

	// axis multiplication by sin
	v3f vs(t.crossProduct(f));

	// axis of rotation
	v3f v(vs);
	v.normalize();

	// cosine angle
	T ca = f.dotProduct(t);

	v3f vt(v * (1 - ca));

	M[0] = vt.X * v.X + ca;
	M[5] = vt.Y * v.Y + ca;
	M[10] = vt.Z * v.Z + ca;

	vt.X *= v.Y;
	vt.Z *= v.X;
	vt.Y *= v.Z;

	M[1] = vt.X - vs.Z;
	M[2] = vt.Z + vs.Y;
	M[3] = 0;

	M[4] = vt.X + vs.Z;
	M[6] = vt.Y - vs.X;
	M[7] = 0;

	M[8] = vt.Z - vs.Y;
	M[9] = vt.Y + vs.X;
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = 0;
	M[15] = 1;

	return *this;
}

//! Builds a matrix which rotates a source vector to a look vector over an arbitrary axis
/** \param camPos: viewer position in world coord
\param center: object position in world-coord, rotation pivot
\param translation: object final translation from center
\param axis: axis to rotate about
\param from: source vector to rotate from
 */
template <class T>
void Matrix4<T>::buildAxisAlignedBillboard(
		const v3f &camPos,
		const v3f &center,
		const v3f &translation,
		const v3f &axis,
		const v3f &from)
{
	// axis of rotation
	v3f up = axis;
	up.normalize();
	const v3f forward = (camPos - center).normalize();
	const v3f right = up.crossProduct(forward).normalize();

	// correct look vector
	const v3f look = right.crossProduct(up);

	// rotate from to
	// axis multiplication by sin
	const v3f vs = look.crossProduct(from);

	// cosine angle
	const f32 ca = from.dotProduct(look);

	v3f vt(up * (1.f - ca));

	M[0] = static_cast<T>(vt.X * up.X + ca);
	M[5] = static_cast<T>(vt.Y * up.Y + ca);
	M[10] = static_cast<T>(vt.Z * up.Z + ca);

	vt.X *= up.Y;
	vt.Z *= up.X;
	vt.Y *= up.Z;

	M[1] = static_cast<T>(vt.X - vs.Z);
	M[2] = static_cast<T>(vt.Z + vs.Y);
	M[3] = 0;

	M[4] = static_cast<T>(vt.X + vs.Z);
	M[6] = static_cast<T>(vt.Y - vs.X);
	M[7] = 0;

	M[8] = static_cast<T>(vt.Z - vs.Y);
	M[9] = static_cast<T>(vt.Y + vs.X);
	M[11] = 0;

	setRotationCenter(center, translation);
}

//! Builds a combined matrix which translate to a center before rotation and translate afterward
template <class T>
void Matrix4<T>::setRotationCenter(const v3f &center, const v3f &translation)
{
	M[12] = -M[0] * center.X - M[4] * center.Y - M[8] * center.Z + (center.X - translation.X);
	M[13] = -M[1] * center.X - M[5] * center.Y - M[9] * center.Z + (center.Y - translation.Y);
	M[14] = -M[2] * center.X - M[6] * center.Y - M[10] * center.Z + (center.Z - translation.Z);
	M[15] = (T)1.0;
}

/*!
	Generate texture coordinates as linear functions so that:
		u = Ux*x + Uy*y + Uz*z + Uw
		v = Vx*x + Vy*y + Vz*z + Vw
	The matrix M for this case is:
		Ux  Vx  0  0
		Uy  Vy  0  0
		Uz  Vz  0  0
		Uw  Vw  0  0
*/

template <class T>
Matrix4<T> &Matrix4<T>::buildTextureTransform(f32 rotateRad,
		const v2f &rotatecenter,
		const v2f &translate,
		const v2f &scale)
{
	const f32 c = cosf(rotateRad);
	const f32 s = sinf(rotateRad);

	M[0] = (T)(c * scale.X);
	M[1] = (T)(s * scale.Y);
	M[2] = 0;
	M[3] = 0;

	M[4] = (T)(-s * scale.X);
	M[5] = (T)(c * scale.Y);
	M[6] = 0;
	M[7] = 0;

	M[8] = (T)(c * scale.X * rotatecenter.X + -s * rotatecenter.Y + translate.X);
	M[9] = (T)(s * scale.Y * rotatecenter.X + c * rotatecenter.Y + translate.Y);
	M[10] = 1;
	M[11] = 0;

	M[12] = 0;
	M[13] = 0;
	M[14] = 0;
	M[15] = 1;

	return *this;
}

// rotate about z axis, center ( 0.5, 0.5 )
template <class T>
Matrix4<T> &Matrix4<T>::setTextureRotationCenter(f32 rotateRad)
{
	const f32 c = cosf(rotateRad);
	const f32 s = sinf(rotateRad);
	M[0] = (T)c;
	M[1] = (T)s;

	M[4] = (T)-s;
	M[5] = (T)c;

	M[8] = (T)(0.5f * (s - c) + 0.5f);
	M[9] = (T)(-0.5f * (s + c) + 0.5f);

	return *this;
}

template <class T>
Matrix4<T> &Matrix4<T>::setTextureTranslate(f32 x, f32 y)
{
	M[8] = (T)x;
	M[9] = (T)y;

	return *this;
}

template <class T>
void Matrix4<T>::getTextureTranslate(f32 &x, f32 &y) const
{
	x = (f32)M[8];
	y = (f32)M[9];
}

template <class T>
Matrix4<T> &Matrix4<T>::setTextureTranslateTransposed(f32 x, f32 y)
{
	M[2] = (T)x;
	M[6] = (T)y;

	return *this;
}

template <class T>
Matrix4<T> &Matrix4<T>::setTextureScale(f32 sx, f32 sy)
{
	M[0] = (T)sx;
	M[5] = (T)sy;

	return *this;
}

template <class T>
void Matrix4<T>::getTextureScale(f32 &sx, f32 &sy) const
{
	sx = (f32)M[0];
	sy = (f32)M[5];
}

template <class T>
Matrix4<T> &Matrix4<T>::setTextureScaleCenter(f32 sx, f32 sy)
{
	M[0] = (T)sx;
	M[5] = (T)sy;
	M[8] = (T)(0.5f - 0.5f * sx);
	M[9] = (T)(0.5f - 0.5f * sy);

	return *this;
}

// sets all matrix data members at once
template <class T>
Matrix4<T> &Matrix4<T>::setM(const T *data)
{
	memcpy(M, data, 16 * sizeof(T));

	return *this;
}

//! Compare two matrices using the equal method
template <class T>
bool Matrix4<T>::equals(const utils::Matrix4<T> &other, const T tolerance) const
{
	for (s32 i = 0; i < 16; ++i)
		if (!utils::equals(M[i], other.M[i], tolerance))
			return false;

	return true;
}

// Multiply by scalar.
template <class T>
Matrix4<T> operator*(const T scalar, const Matrix4<T> &mat)
{
	return mat * scalar;
}

}
