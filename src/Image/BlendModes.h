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
        /*ErrorStream << "BlendModes function: the source and destination images formats have to be equal\n"; \*/ \
        return color8(src_c.getFormat(), 0, 0, 0, 0); \
    }

#define BLEND_OP(res_c, dst_a) \
    color8 res_copy(res_c); \
    res_copy.A(dst_a); \
    return res_copy;


inline color8 NormalBlend(const color8 &src, const color8 &dst)
{
	return src;
}

inline color8 AlphaBlend(const color8 &src, const color8 &dst)
{
    u32 src_a = src.A();
    u32 dst_a = dst.A();
    u32 max_v = 255;

    if (dst_a == 255) {
        BLEND_OP((src * src_a + dst * (max_v - src_a)) / max_v, dst_a);
    }
    else {
        u8 res_a = src_a + dst_a * (max_v - src_a);

        if (res_a == 0)
            return color8(src.getFormat(), 0, 0, 0, 0);

        BLEND_OP((src * src_a + dst * dst_a * (max_v - src_a) / max_v) / (res_a * max_v), res_a);
    }
}

inline color8 AddBlend(const color8 &src, const color8 &dst)
{
	return src + dst;
}

inline color8 SubtractBlend(const color8 &src, const color8 &dst)
{
	return src - dst;
}

inline color8 MultiplyBlend(const color8 &src, const color8 &dst)
{
    return src * dst;
}

inline color8 DivisionBlend(const color8 &src, const color8 &dst)
{
    return src / dst;
}

inline color8 ScreenBlend(const color8 &src, const color8 &dst)
{
    CHECK_SAME_FORMAT(src, dst);

    u8 max_v = 255;
    color8 max_c(src.getFormat(), max_v, max_v, max_v);
    BLEND_OP(max_c - (max_c - src)*(max_c - dst), dst.A());
}

inline color8 OverlayBlend(const color8 &src, const color8 &dst)
{
    u8 max_v = 255;

    color8 newColor(src.getFormat(), 0, 0, 0, dst.A());

	if (src.R() < max_v / 2)
        newColor.R(2 * src.R() * dst.R() / (f32)max_v);
	else
        newColor.R(max_v - 2*(max_v - src.R())*(max_v - dst.R()) / (f32)max_v);
	if (src.G() < max_v / 2)
        newColor.G(2 * src.G() * dst.G() / (f32)max_v);
	else
        newColor.G(max_v - 2*(max_v - src.G())*(max_v - dst.G()) / (f32)max_v);
	if (src.B() < max_v / 2)
        newColor.B(2 * src.B() * dst.B() / (f32)max_v);
	else
        newColor.B(max_v - 2*(max_v - src.B())*(max_v - dst.B()) / (f32)max_v);

	return newColor;
}

inline color8 HardLightBlend(const color8 &src, const color8 &dst)
{
    u8 max_v = 255;

    color8 newColor(src.getFormat(), 0, 0, 0, dst.A());

	if (dst.R() < max_v / 2)
        newColor.R(2 * src.R() * dst.R() / (f32)max_v);
	else
        newColor.R(max_v - 2*(max_v - src.R())*(max_v - dst.R()) / (f32)max_v);
	if (dst.G() < max_v / 2)
        newColor.G(2 * src.G() * dst.G() / (f32)max_v);
	else
        newColor.G(max_v - 2*(max_v - src.G())*(max_v - dst.G()) / (f32)max_v);
	if (dst.B() < max_v / 2)
        newColor.B(2 * src.B() * dst.B() / (f32)max_v);
	else
        newColor.B(max_v - 2*(max_v - src.B())*(max_v - dst.B()) / (f32)max_v);

	return newColor;
}

inline color8 SoftLightBlend(const color8 &src, const color8 &dst)
{
    u8 max_v = 255;
    color8 max_c(src.getFormat(), max_v, max_v, max_v);
    BLEND_OP((max_c - dst*2)*src*src + dst*src*2, dst.A());
}

inline color8 GrainExtractBlend(const color8 &src, const color8 &dst)
{
    u8 max_v = 255;

    BLEND_OP(dst - src + max_v/2, dst.A());
}

inline color8 GrainMergeBlend(const color8 &src, const color8 &dst)
{
    CHECK_SAME_FORMAT(src, dst);

    u8 max_v = 255;

    BLEND_OP(dst + src - max_v/2, dst.A());
}

inline color8 DarkenOnlyBlend(const color8 &src, const color8 &dst)
{
    return color8(
		src.getFormat(),
        std::min<u8>(src.R(), dst.R()),
        std::min<u8>(src.G(), dst.G()),
        std::min<u8>(src.B(), dst.B()),
        dst.A()
	);
}

inline color8 LightenOnlyBlend(const color8 &src, const color8 &dst)
{
    return color8(
		src.getFormat(),
        std::max<u8>(src.R(), dst.R()),
        std::max<u8>(src.G(), dst.G()),
        std::max<u8>(src.B(), dst.B()),
        dst.A()
	);
}


color8 doBlend(const color8 &src, const color8 &dst, BlendMode mode);

}
