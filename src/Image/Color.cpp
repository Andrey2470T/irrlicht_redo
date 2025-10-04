#include "Color.h"

namespace img
{

bool isFormatSupportedForImage(PixelFormat format)
{
    switch (format) {
    case PF_R8:
    case PF_RG8:
    case PF_RGB8:
    case PF_RGBA8:
    case PF_INDEX_RGBA8:
        return true;
    default:
        return false;
    }
}

u8 addCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2;

    return std::clamp<u32>(ch1_32 + ch2_32, 0, 255);
}

u8 subCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2;

    return std::clamp<u32>(ch1_32 - ch2_32, 0, 255);
}

u8 mulCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2;

    return std::clamp<u32>(ch1_32 * ch2_32 / 255.0f, 0, 255);
}

u8 divCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2 == 0 ? 1 : ch2;

    return std::clamp<u32>(ch1_32 / ch2_32 * 255.0f, 0, 255);
}

// Named (often used) colors
color8 white = color8(PF_RGBA8, 255, 255, 255, 255);
color8 black = color8(PF_RGBA8, 0, 0, 0, 255);
color8 gray = color8(PF_RGBA8, 128, 128, 128, 255);
color8 red = color8(PF_RGBA8, 255, 0, 0, 255);
color8 green = color8(PF_RGBA8, 0, 255, 0, 255);
color8 blue = color8(PF_RGBA8, 0, 0, 255, 255);
color8 yellow = color8(PF_RGBA8, 255, 255, 0, 255);
color8 violet = color8(PF_RGBA8, 238, 130, 238, 255);
color8 cyan = color8(PF_RGBA8, 0, 255, 255, 255);
color8 orange = color8(PF_RGBA8, 255, 165, 0, 255);
color8 pink = color8(PF_RGBA8, 255, 192, 203, 255);
color8 brown = color8(PF_RGBA8, 165, 42, 42, 255);

inline void ColorHSL::fromRGBA(const colorf &color)
{
    const f32 maxVal = utils::max3<f32>(color.R(), color.G(), color.B());
    const f32 minVal = utils::min3<f32>(color.R(), color.G(), color.B());

	L = (maxVal + minVal) * 50;

    if (utils::equals(maxVal, minVal)) {
		H = 0.0f;
		S = 0.0f;
		return;
	}

	const f32 delta = maxVal - minVal;
	if (L <= 50) {
		S = (delta) / (maxVal + minVal);
	} else {
		S = (delta) / (2 - maxVal - minVal);
	}
	S *= 100;

    if (utils::equals(maxVal, color.R()))
        H = (color.G() - color.B()) / delta;
    else if (utils::equals(maxVal, color.G()))
        H = 2 + ((color.B() - color.R()) / delta);
	else // blue is max
        H = 4 + ((color.R() - color.G()) / delta);

	H *= 60.0f;
	while (H < 0.0f)
		H += 360;
}

inline void ColorHSL::toRGBA(colorf &color) const
{
	const f32 l = L / 100;
    if (utils::equals(S, 0.0f)) { // grey
        color.R(l);
        color.G(l);
        color.B(l);
		return;
	}

	f32 rm2;

	if (L <= 50) {
		rm2 = l + l * (S / 100);
	} else {
		rm2 = l + (1 - l) * (S / 100);
	}

	const f32 rm1 = 2.0f * l - rm2;

	const f32 h = H / 360.0f;
    color.R(toRGBA1(rm1, rm2, h + 1.0f / 3.0f));
    color.G(toRGBA1(rm1, rm2, h));
    color.B(toRGBA1(rm1, rm2, h - 1.0f / 3.0f));
}

// algorithm from Foley/Van-Dam
inline f32 ColorHSL::toRGBA1(f32 rm1, f32 rm2, f32 rh) const
{
	if (rh < 0)
		rh += 1;
	if (rh > 1)
		rh -= 1;

	if (rh < 1.f / 6.f)
		rm1 = rm1 + (rm2 - rm1) * rh * 6.f;
	else if (rh < 0.5f)
		rm1 = rm2;
	else if (rh < 2.f / 3.f)
		rm1 = rm1 + (rm2 - rm1) * ((2.f / 3.f) - rh) * 6.f;

	return rm1;
}

color8 getColor8(const ByteArray *arr, u32 n, img::PixelFormat format)
{
    if (format != img::PF_RGB8 && format != img::PF_RGBA8) {
        ErrorStream << "getColor8() unsupported format\n";
        return color8();
    }
    color8 c(format);

	c.R(arr->getUInt8(n));
	c.G(arr->getUInt8(n+1));
	c.B(arr->getUInt8(n+2));

    if (format == img::PF_RGBA8) c.A(arr->getUInt8(n+3));

	return c;
}

void setColor8(ByteArray *arr, const color8 &c, u32 n, img::PixelFormat format)
{
    if (format != img::PF_RGB8 && format != img::PF_RGBA8) {
        ErrorStream << "setColor8() unsupported format\n";
        return;
    }
	arr->setUInt8(c.R(), n);
    arr->setUInt8(c.G(), n+1);
    arr->setUInt8(c.B(), n+2);
    if (format == img::PF_RGBA8) arr->setUInt8(c.A(), n+3);
}

}
