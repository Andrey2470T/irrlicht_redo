#pragma once

#include "Color.h"

namespace img
{

enum BlendMode : u8
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

template <class T>
inline ColorRGBA<T> NormalBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src;
}

template <class T>
inline ColorRGBA<T> AlphaBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.getFormat(),
        src.R() * src.A() + dst.R() * (T_MAX(T) - src.A()),
        src.G() * src.A() + dst.G() * (T_MAX(T) - src.A()),
        src.B() * src.A() + dst.B() * (T_MAX(T) - src.A()),
		src.A()
	);
}

template <class T>
inline ColorRGBA<T> AddBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src + dst;
}

template <class T>
inline ColorRGBA<T> SubtractBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src - dst;
}

template <class T>
inline ColorRGBA<T> MultiplyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src * dst;
}

template <class T>
inline ColorRGBA<T> DivisionBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.getFormat(),
		src.R() / dst.R(),
		src.G() / dst.G(),
		src.B() / dst.B(),
		src.A() / dst.A()
	);
}

template <class T>
inline ColorRGBA<T> ScreenBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);
	return ColorRGBA<T>(
		src.getFormat(),
		max_v - (max_v - src.R())*(max_v - dst.R()),
		max_v - (max_v - src.G())*(max_v - dst.G()),
		max_v - (max_v - src.B())*(max_v - dst.B())
	);
}

template <class T>
inline ColorRGBA<T> OverlayBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

	ColorRGBA<T> newColor(src.getFormat());

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

template <class T>
inline ColorRGBA<T> HardLightBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

	ColorRGBA<T> newColor(src.getFormat());

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

template <class T>
inline ColorRGBA<T> SoftLightBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

	return ColorRGBA<T>(
		src.getFormat(),
		(max_v - 2*dst.R())*src.R()*src.R() + 2*dst.R()*src.R(),
		(max_v - 2*dst.G())*src.G()*src.G() + 2*dst.G()*src.G(),
		(max_v - 2*dst.B())*src.B()*src.B() + 2*dst.B()*src.B()
	);
}

template <class T>
inline ColorRGBA<T> GrainExtractBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

	return dst - src + max_v/2;
}

template <class T>
inline ColorRGBA<T> GrainMergeBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

	return dst + src - max_v/2;
}

template <class T>
inline ColorRGBA<T> DarkenOnlyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.getFormat(),
		std::min<T>(src.R(), dst.R()),
		std::min<T>(src.G(), dst.G()),
		std::min<T>(src.B(), dst.B())
	);
}

template <class T>
inline ColorRGBA<T> LightenOnlyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.getFormat(),
		std::max<T>(src.R(), dst.R()),
		std::max<T>(src.G(), dst.G()),
		std::max<T>(src.B(), dst.B())
	);
}


template <class T>
ColorRGBA<T> doBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst, BlendMode mode)
{
	switch (mode) {
		case BM_NORMAL:
			return NormalBlend(src, dst);
		case BM_ALPHA:
			return AlphaBlend(src, dst);
		case BM_ADD:
			return AddBlend(src, dst);
		case BM_SUBTRACTION:
			return SubtractBlend(src, dst);
		case BM_MULTIPLY:
			return MultiplyBlend(src, dst);
		case BM_DIVISION:
			return DivisionBlend(src, dst);
		case BM_SCREEN:
			return ScreenBlend(src, dst);
		case BM_OVERLAY:
			return OverlayBlend(src, dst);
		case BM_HARD_LIGHT:
			return HardLightBlend(src, dst);
		case BM_SOFT_LIGHT:
			return SoftLightBlend(src, dst);
		case BM_GRAIN_EXTRACT:
			return GrainExtractBlend(src, dst);
		case BM_GRAIN_MERGE:
			return GrainMergeBlend(src, dst);
		case BM_DARKEN_ONLY:
			return DarkenOnlyBlend(src, dst);
		case BM_LIGHTEN_ONLY:
			return LightenOnlyBlend(src, dst);
		default:
			return ColorRGBA<T>(src.getFormat());
	}
}

}
