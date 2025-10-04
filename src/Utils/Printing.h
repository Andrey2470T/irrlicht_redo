#pragma once

#include "BasicIncludes.h"
#include "AABB.h"
#include "Line2D.h"
#include "Line3D.h"
#include "Rect.h"
#include "Image/Color.h"
#include <ostream>

namespace utils {

template<typename T>
std::ostream &operator<<(std::ostream &os, Vector2D<T> v)
{
    return os << "( X: " << v.X << ", Y: " << v.Y << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, Vector3D<T> v)
{
    return os << "( X: " << v.X << ", Y: " << v.Y << ", Z: " << v.Z << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, AABB<T> aabb)
{
    return os << "( MinEdge: " << aabb.MinEdge << ", MaxEdge: " << aabb.MaxEdge << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, Line2D<T> l)
{
    return os << "( Start: " << l.Start << ", End: " << l.End << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, Line3D<T> l)
{
    return os << "( Start: " << l.Start << ", End: " << l.End << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, Rect<T> r)
{
    return os << "( UpperLeftCorner: " << r.ULC  << ", LowerRightCorner: " << r.LRC << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, img::color8 c)
{
    return os << "( R: " << c.R()  << ", G: " << c.G() << ", B: " << c.B() << ", A: " << c.A() << " )";
}

template<typename T>
std::ostream &operator<<(std::ostream &os, img::colorf c)
{
    return os << "( R: " << c.R()  << ", G: " << c.G() << ", B: " << c.B() << ", A: " << c.A() << " )";
}

};
