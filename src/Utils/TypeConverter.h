#pragma once

#include "Types.h"
#include "Rect.h"

namespace utils
{

template<typename T1, typename T2>
Vector2D<T1> toV2T(const Vector2D<T2> &v)
{
    return Vector2D<T1>(v.X, v.Y);
}

template<typename T1, typename T2>
Vector3D<T1> toV3T(const Vector3D<T2> &v)
{
    return Vector3D<T1>(v.X, v.Y, v.Z);
}

template<typename T1, typename T2>
Rect<T1> toRectT(const Rect<T2> &r)
{
    return Rect<T1>(r.ULC.X, r.ULC.Y, r.LRC.X, r.LRC.Y);
}

}
