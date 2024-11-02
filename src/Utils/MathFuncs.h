#pragma once

#include <cmath>
#include "Typedefs.h"

namespace utils
{

#ifndef PI
//! Constant for PI.
const f64 PI = std::atan(1.0) * 4;
#endif

//! Constant for converting from degrees to radians
static const f64 DEGTORAD = PI / 180.0;

//! Constant for converting from radians to degrees
static const f64 RADTODEG = 180.0 / PI;

//! Rounding error constants
static const f32 ROUNDING_ERROR_f32 = 0.000001f;
static const f64 ROUNDING_ERROR_f64 = 0.00000001;

//! Utility function to convert a radian value to degrees
/** Provided as it can be clearer to write radToDeg(X) than RADTODEG * X
\param radians The radians value to convert to degrees.
*/
inline f64 radToDeg(f64 radians)
{
	return RADTODEG * radians;
}

//! Utility function to convert a degrees value to radians
/** Provided as it can be clearer to write degToRad(X) than DEGTORAD * X
\param degrees The degrees value to convert to radians.
*/
inline f64 degToRad(f64 degrees)
{
	return DEGTORAD * degrees;
}

//! Does linear interpolation of a and b with ratio t
//! \return: a if t==0, b if t==1, and the linear interpolation else
template <class T>
inline T lerp(const T a, const T b, const f32 t)
{
	return (T)(a * (1.0f - t)) + (b * t);
}

template <class T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
inline bool equals(const T a, const T b)
{
	return a == b;
}

inline bool equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return std::abs(a - b) <= tolerance;
}

inline bool equals(const f64 a, const f64 b, const f32 tolerance = ROUNDING_ERROR_f64)
{
	return std::abs(a - b) <= tolerance;
}

}
