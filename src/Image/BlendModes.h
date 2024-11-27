#pragma once

#include "BasicIncludes.h"
#include <functional>

namespace img
{

enum BLEND_MODE
{
	BM_NORMAL = 0,
	BM_ALPHA,
	BM_ADD,
	BM_SUBTRACTION,
	BM_MULTIPLY,
	BM_DIVISION,
	BM_SCREEN,
	BM_OVERLAY,
	BM_HARD_LIGHT,
	BM_SOFT_LIGHT,
	BM_GRAIN_EXTRACT,
	BM_GRAIN_MERGE,
	BM_DARKEN_ONLY,
	BM_LIGHTEN_ONLY,
	BM_COUNT
};

inline ColorRGBA<T> NormalBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src;
}

inline ColorRGBA<T> AlphaBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.R() * src.A() + dst.R() * (max_v - src.A()),
		src.G() * src.A() + dst.G() * (max_v - src.A()),
		src.B() * src.A() + dst.B() * (max_v - src.A()),
		src.A()
	);
}

inline ColorRGBA<T> AddBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src + dst;
}

inline ColorRGBA<T> SubtractBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src - dst;
}

inline ColorRGBA<T> MultiplyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src * dst;
}

inline ColorRGBA<T> DivisionBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.R() / dst.R(),
		src.G() / dst.G(),
		src.B() / dst.B(),
		src.A() / dst.A()
	);
}

inline ColorRGBA<T> ScreenBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = std::numeric_limits<T>::max();
	return ColorRGBA<T>(
		max_v - (max_v - src.R())*(max_v - dst.R()),
		max_v - (max_v - src.G())*(max_v - dst.G()),
		max_v - (max_v - src.B())*(max_v - dst.B())
	);
}

inline ColorRGBA<T> OverlayBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = std::numeric_limits<T>::max();

	ColorRGBA<T> newColor;

	if (src.R() < max_v / 2)
		newColor.R(2 * src.R() * dst.R());
	else
		newColor.R(max_v - 2*(max_v - src.R())*(max_v - dst.R()));
	if (src.G() < max_v / 2)
		newColor.G(2 * src.G() * dst.G());
	else
		newColor.G(max_v - 2*(max_v - src.G())*(max_v - dst.G()));
	if (src.B() < max_v / 2)
		newColor.B(2 * src.B() * dst.B());
	else
		newColor.B(max_v - 2*(max_v - src.B())*(max_v - dst.B()));

	return newColor;
}

inline ColorRGBA<T> HardLightBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = std::numeric_limits<T>::max();

	ColorRGBA<T> newColor;

	if (dst.R() < max_v / 2)
		newColor.R(2 * src.R() * dst.R());
	else
		newColor.R(max_v - 2*(max_v - src.R())*(max_v - dst.R()));
	if (dst.G() < max_v / 2)
		newColor.G(2 * src.G() * dst.G());
	else
		newColor.G(max_v - 2*(max_v - src.G())*(max_v - dst.G()));
	if (dst.B() < max_v / 2)
		newColor.B(2 * src.B() * dst.B());
	else
		newColor.B(max_v - 2*(max_v - src.B())*(max_v - dst.B()));

	return newColor;
}

inline ColorRGBA<T> SoftLightBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = std::numeric_limits<T>::max();

	return ColorRGBA<T>(
		(max_v - 2*dst.R())*src.R()*src.R() + 2*dst.R()*src.R(),
		(max_v - 2*dst.G())*src.G()*src.G() + 2*dst.G()*src.G(),
		(max_v - 2*dst.B())*src.B()*src.B() + 2*dst.B()*src.B()
	);
}

inline ColorRGBA<T> GrainExtractBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = std::numeric_limits<T>::max();

	return dst - src + max_v/2;
}

inline ColorRGBA<T> GrainMergeBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = std::numeric_limits<T>::max();

	return dst + src - max_v/2;
}

inline ColorRGBA<T> DarkenOnlyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		std::min<T>(src.R(), dst.R()),
		std::min<T>(src.G(), dst.G()),
		std::min<T>(src.B(), dst.B())
	);
}

inline ColorRGBA<T> LightenOnlyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		std::max<T>(src.R(), dst.R()),
		std::max<T>(src.G(), dst.G()),
		std::max<T>(src.B(), dst.B())
	);
}
