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

#define CHECK_SAME_FORMAT(src_c, dst_c) \
    if (src_c.getFormat() != dst_c.getFormat()) {\
        ErrorStream << "BlendModes function: the source and destination images formats have to be equal\n"; \
        return ColorRGBA<T>(src_c.getFormat(), 0, 0, 0, 0); \
    }

#define BLEND_OP(res_c, dst_a) \
    ColorRGBA<T> res_copy(res_c); \
    res_copy.A(dst_a); \
    return res_copy;


template <class T>
inline ColorRGBA<T> NormalBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return src;
}

template <class T>
inline ColorRGBA<T> AlphaBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
    T src_a = src.A();
    T dst_a = dst.A();
    T max_v = T_MAX(T);

    if (dst_a == 255) {
        BLEND_OP((src * src_a + dst * (max_v - src_a)) / max_v, dst_a);
    }
    else {
        T res_a = src_a + dst_a * (max_v - src_a);

        if (res_a == 0)
            return ColorRGBA<T>(src.getFormat(), 0, 0, 0, 0);

        BLEND_OP((src * src_a + dst * dst_a * (max_v - src_a) / max_v) / (res_a * max_v), res_a);
    }
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
    return (src * dst) / T_MAX(T);
}

template <class T>
inline ColorRGBA<T> DivisionBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
    return (src / dst) * T_MAX(T);
}

template <class T>
inline ColorRGBA<T> ScreenBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
    CHECK_SAME_FORMAT(src, dst);

	T max_v = T_MAX(T);
    ColorRGBA<T> max_c(src.getFormat(), max_v, max_v, max_v);
    BLEND_OP(max_c - (max_c - src)*(max_c - dst) / max_v, dst.A());
}

template <class T>
inline ColorRGBA<T> OverlayBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

    ColorRGBA<T> newColor(src.getFormat(), 0, 0, 0, dst.A());

	if (src.R() < max_v / 2)
        newColor.R(2 * src.R() * dst.R() / max_v);
	else
        newColor.R(max_v - 2*(max_v - src.R())*(max_v - dst.R()) / max_v);
	if (src.G() < max_v / 2)
        newColor.G(2 * src.G() * dst.G() / max_v);
	else
        newColor.G(max_v - 2*(max_v - src.G())*(max_v - dst.G()) / max_v);
	if (src.B() < max_v / 2)
        newColor.B(2 * src.B() * dst.B() / max_v);
	else
        newColor.B(max_v - 2*(max_v - src.B())*(max_v - dst.B()) / max_v);

	return newColor;
}

template <class T>
inline ColorRGBA<T> HardLightBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

    ColorRGBA<T> newColor(src.getFormat(), 0, 0, 0, dst.A());

	if (dst.R() < max_v / 2)
        newColor.R(2 * src.R() * dst.R() / max_v);
	else
        newColor.R(max_v - 2*(max_v - src.R())*(max_v - dst.R()) / max_v);
	if (dst.G() < max_v / 2)
        newColor.G(2 * src.G() * dst.G() / max_v);
	else
        newColor.G(max_v - 2*(max_v - src.G())*(max_v - dst.G()) / max_v);
	if (dst.B() < max_v / 2)
        newColor.B(2 * src.B() * dst.B() / max_v);
	else
        newColor.B(max_v - 2*(max_v - src.B())*(max_v - dst.B()) / max_v);

	return newColor;
}

template <class T>
inline ColorRGBA<T> SoftLightBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);
    ColorRGBA<T> max_c(src.getFormat(), max_v, max_v, max_v);
    BLEND_OP(((max_c - dst*2)*src*src + dst*src*2) / max_v, dst.A());
}

template <class T>
inline ColorRGBA<T> GrainExtractBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	T max_v = T_MAX(T);

    BLEND_OP(dst - src + max_v/2, dst.A());
}

template <class T>
inline ColorRGBA<T> GrainMergeBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
    CHECK_SAME_FORMAT(src, dst);

	T max_v = T_MAX(T);

    BLEND_OP(dst + src - max_v/2, dst.A());
}

template <class T>
inline ColorRGBA<T> DarkenOnlyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.getFormat(),
		std::min<T>(src.R(), dst.R()),
		std::min<T>(src.G(), dst.G()),
        std::min<T>(src.B(), dst.B()),
        dst.A()
	);
}

template <class T>
inline ColorRGBA<T> LightenOnlyBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst)
{
	return ColorRGBA<T>(
		src.getFormat(),
		std::max<T>(src.R(), dst.R()),
		std::max<T>(src.G(), dst.G()),
        std::max<T>(src.B(), dst.B()),
        dst.A()
	);
}


template <class T>
ColorRGBA<T> doBlend(const ColorRGBA<T> &src, const ColorRGBA<T> &dst, BlendMode mode)
{
    CHECK_SAME_FORMAT(src, dst);

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
