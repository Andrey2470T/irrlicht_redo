#pragma once

#include "BasicIncludes.h"
#include "AABB.h"
#include "Line2D.h"
#include "Line3D.h"
#include "Rect.h"
#include <ostream>

namespace utils
{

template<typename T>
std::ostream &operator<<(std::ostream &os, const Vector2D<T> &v)
{
    return os << "( X: " << v.X << ", Y: " << v.Y << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const Vector3D<T> &v)
{
    return os << "( X: " << v.X << ", Y: " << v.Y << ", Z: " << v.Z << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const AABB<T> &aabb)
{
    return os << "( MinEdge: " << aabb.MinEdge << ", MaxEdge: " << aabb.MaxEdge << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const Line2D<T> &l)
{
    return os << "( Start: " << l.Start << ", End: " << l.End << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const Line3D<T> &l)
{
    return os << "( Start: " << l.Start << ", End: " << l.End << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, const Rect<T> &r)
{
    return os << "( UpperLeftCorner: " << r.ULC  << ", LowerRightCorner: " << r.LRC << " )";
}

}