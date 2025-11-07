#include "Color.h"

namespace img
{

bool isFormatSupportedForColor8(PixelFormat format)
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

static u8 addCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2;

    return std::clamp<u32>(ch1_32 + ch2_32, 0, 255);
}

static u8 subCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2;

    return std::clamp<u32>(ch1_32 - ch2_32, 0, 255);
}

static u8 mulCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2;

    return std::clamp<u32>(ch1_32 * ch2_32 / 255.0f, 0, 255);
}

static u8 divCh(u8 ch1, u8 ch2)
{
    u32 ch1_32 = ch1;
    u32 ch2_32 = ch2 == 0 ? 1 : ch2;

    return std::clamp<u32>(ch1_32 / ch2_32 * 255.0f, 0, 255);
}

color8::color8()
    : format(PF_RGBA8), color(pixelFormatInfo[PF_RGBA8].channels, pixelFormatInfo[PF_RGBA8].size / 8)
{
    set(0, 0, 0, 0);
}

color8::color8(PixelFormat _format)
    : format(_format), color(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8)
{
    assert(isFormatSupportedForColor8(format));
    set(0, 0, 0, 0);
}

color8::color8(PixelFormat _format, u8 _R, u8 _G, u8 _B, u8 _A)
    : format(_format), color(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8)
{
    assert(isFormatSupportedForColor8(format));
    set(_R, _G, _B, _A);
}

color8::color8(PixelFormat _format, u32 colorNum)
    : format(_format), color(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8)
{
    assert(isFormatSupportedForColor8(format));

    u8 a = colorNum >> 24;
    u8 r = (colorNum >> 16) & 0xFF;
    u8 g = (colorNum >> 8) & 0xFF;
    u8 b = colorNum & 0xFF;

    set(r, g, b, a);
}

color8::color8(const color8 &other)
    : format(other.format), color(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8)
{
    assert(isFormatSupportedForColor8(format));
    set(other.R(), other.G(), other.B(), other.A());
}

color8 &color8::operator=(const color8 &other)
{
    format = other.getFormat();
    assert(isFormatSupportedForColor8(format));

    color.reallocate(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8);
    set(other.R(), other.G(), other.B(), other.A());

    return *this;
}

//! Get lightness of the color
f32 color8::getLightness() const
{
    u8 channelsCount = pixelFormatInfo[format].channels;

    if (channelsCount == 1)
        return 0.5f * R();
    else if (channelsCount == 2)
        return 0.5f * (std::max(R(), G()) + std::min(R(), G()));
    else
        return 0.5f * (addCh(max3(R(), G(), B()), min3(R(), G(), B())));
}

//! Get luminance of the color
f32 color8::getLuminance() const
{
    if (pixelFormatInfo[format].channels < 3)
        return 0.0f;
    return 0.3f * R() + 0.59f * G() + 0.11f * B();
}

//! Get average intensity of the color
u32 color8::getAverage() const
{
    u8 channelsCount = pixelFormatInfo[format].channels;

    if (channelsCount == 1)
        return R();
    else if (channelsCount == 2)
        return addCh(R(), G()) / 2;
    else
        return addCh(addCh(R(), G()), B()) / 3;
}

bool color8::operator==(const color8 &other) const
{
    u8 channelsCount = pixelFormatInfo[format].channels;

    if (channelsCount == 1)
        return R() == other.R();
    else if (channelsCount == 2)
        return (R() == other.R() && G() == other.G());
    else if (channelsCount == 3)
        return (R() == other.R() && G() == other.G() && B() == other.B());
    else
        return (R() == other.R() && G() == other.G() && B() == other.B() && A() == other.A());
}

bool color8::operator!=(const color8 &other) const
{
    return !(*this == other);
}

bool color8::operator<(const color8 &other) const
{
    u8 channelsCount = pixelFormatInfo[format].channels;

    if (channelsCount == 1)
        return R() <= other.R();
    else if (channelsCount == 2)
        return (R() <= other.R() && G() <= other.G());
    else if (channelsCount == 3)
        return (R() <= other.R() && G() <= other.G() && B() <= other.B());
    else
        return (R() <= other.R() && G() <= other.G() && B() <= other.B() && A() <= other.A());
}

color8 color8::operator+(const color8 &other) const
{
    return color8(
        getFormat(),
        addCh(R(), other.R()),
        addCh(G(), other.G()),
        addCh(B(), other.B()),
        other.A());
}

color8 &color8::operator+=(const color8 &other)
{
    R(addCh(R(), other.R()));
    G(addCh(G(), other.G()));
    B(addCh(B(), other.B()));
    A(other.A());

    return *this;
}

color8 color8::operator+(u8 val) const
{
    return color8(
        getFormat(),
        addCh(R(), val),
        addCh(G(), val),
        addCh(B(), val),
        A());
}

color8 &color8::operator+=(u8 val)
{
    R(addCh(R(), val));
    G(addCh(G(), val));
    B(addCh(B(), val));

    return *this;
}

color8 color8::operator-(const color8 &other) const
{
    return color8(
        getFormat(),
        subCh(R(), other.R()),
        subCh(G(), other.G()),
        subCh(B(), other.B()),
        other.A());
}

color8 &color8::operator-=(const color8 &other)
{
    R(subCh(R(), other.R()));
    G(subCh(G(), other.G()));
    B(subCh(B(), other.B()));
    A(other.A());

    return *this;
}

color8 color8::operator-(u8 val) const
{
    return color8(
        getFormat(),
        subCh(R(), val),
        subCh(G(), val),
        subCh(B(), val),
        A());
}

color8 &color8::operator-=(u8 val)
{
    R(subCh(R(), val));
    G(subCh(G(), val));
    B(subCh(B(), val));

    return *this;
}

color8 color8::operator*(const color8 &other) const
{
    return color8(
        getFormat(),
        mulCh(R(), other.R()),
        mulCh(G(), other.G()),
        mulCh(B(), other.B()),
        other.A());
}

color8 &color8::operator*=(const color8 &other)
{
    R(mulCh(R(), other.R()));
    G(mulCh(G(), other.G()));
    B(mulCh(B(), other.B()));
    B(other.A());

    return *this;
}

color8 color8::operator*(u8 val) const
{
    return color8(
        getFormat(),
        mulCh(R(), val),
        mulCh(G(), val),
        mulCh(B(), val),
        A());
}

color8 &color8::operator*=(u8 val)
{
    R(mulCh(R(), val));
    G(mulCh(G(), val));
    B(mulCh(B(), val));

    return *this;
}

color8 color8::operator/(const color8 &other) const
{
    return color8(
        getFormat(),
        divCh(R(), other.R()),
        divCh(G(), other.G()),
        divCh(B(), other.B()),
        other.A());
}

color8 &color8::operator/=(const color8 &other)
{
    R(divCh(R(), other.R()));
    G(divCh(G(), other.G()));
    B(divCh(B(), other.B()));
    B(other.A());

    return *this;
}

color8 color8::operator/(u8 val) const
{
    return color8(
        getFormat(),
        divCh(R(), val),
        divCh(G(), val),
        divCh(B(), val),
        A());
}

color8 &color8::operator/=(u8 val)
{
    R(divCh(R(), val));
    G(divCh(G(), val));
    B(divCh(B(), val));

    return *this;
}

//! Interpolates the color with a f32 value to another color
/** \param other: Other color
\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
\return Interpolated color. */
color8 color8::linInterp(const color8 &other, f32 d) const
{
    return color8(
        getFormat(),
        lerp<u8>(other.R(), R(), d),
        lerp<u8>(other.G(), G(), d),
        lerp<u8>(other.B(), B(), d),
        A());
}

//! Returns interpolated color. ( quadratic )
/** \param c1: first color to interpolate with
\param c2: second color to interpolate with
\param d: value between 0.0f and 1.0f. */
color8 color8::quadInterp(const color8 &c1, const color8 &c2, f32 d) const
{
    return color8(
        getFormat(),
        qerp<u8>(R(), c1.R(), c2.R(), d),
        qerp<u8>(G(), c1.G(), c2.G(), d),
        qerp<u8>(B(), c1.B(), c2.B(), d),
        A());
}

// res = c1 * d + c2 * (1- d)
color8 color8::linInterp(const color8 &c1, const color8 &c2, f32 d)
{
    return color8(
        c1.getFormat(),
        lerp<u8>(c2.R(), c1.R(), d),
        lerp<u8>(c2.G(), c1.G(), d),
        lerp<u8>(c2.B(), c1.B(), d),
        c1.A());
}

void color8::set(u8 R, u8 G, u8 B, u8 A)
{
    u8 channelsCount = pixelFormatInfo[format].channels;

    setChannel(R, 0);

    if (channelsCount > 1)
        setChannel(G, 1);
    if (channelsCount > 2)
        setChannel(B, 2);
    if (channelsCount > 3)
        setChannel(A, 3);
}

u8 color8::getChannel(u32 n) const
{
    auto channelsCount = pixelFormatInfo[format].channels;

    if (n >= channelsCount)
        return 0;

    return color.getUInt8(n);
}

void color8::setChannel(u8 v, u32 n)
{
    auto channelsCount = pixelFormatInfo[format].channels;

    if (n >= channelsCount)
        return;

    color.setUInt8(v, n);
}


colorf colorf::linInterp(const colorf &other, f32 d) const
{
    return colorf(
        lerp<f32>(other.R(), R(), d),
        lerp<f32>(other.G(), G(), d),
        lerp<f32>(other.B(), B(), d),
        A());
}

//! Returns interpolated color. ( quadratic )
/** \param c1: first color to interpolate with
\param c2: second color to interpolate with
\param d: value between 0.0f and 1.0f. */
colorf colorf::quadInterp(const colorf &c1, const colorf &c2, f32 d) const
{
    return colorf(
        qerp<f32>(R(), c1.R(), c2.R(), d),
        qerp<f32>(G(), c1.G(), c2.G(), d),
        qerp<f32>(B(), c1.B(), c2.B(), d),
        A());
}

// res = c1 * d + c2 * (1- d)
colorf colorf::linInterp(const colorf &c1, const colorf &c2, f32 d)
{
    return colorf(
        lerp<f32>(c2.R(), c1.R(), d),
        lerp<f32>(c2.G(), c1.G(), d),
        lerp<f32>(c2.B(), c1.B(), d),
        c1.A());
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

void ColorHSL::fromRGBA(const colorf &color)
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

void ColorHSL::toRGBA(colorf &color) const
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
f32 ColorHSL::toRGBA1(f32 rm1, f32 rm2, f32 rh) const
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

    if (format == img::PF_RGBA8) {
        c.A(arr->getUInt8(n+3));
    }

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

    if (format == img::PF_RGBA8)
        arr->setUInt8(c.A(), n+3);
}

colorf getColorF32(const ByteArray *arr, u32 n)
{
    colorf c;

    c.R(arr->getFloat(n));
    c.G(arr->getFloat(n+1));
    c.B(arr->getFloat(n+2));
    c.A(arr->getFloat(n+3));

    return c;
}

void setColorF32(ByteArray *arr, const colorf &c, u32 n)
{
    arr->setFloat(c.R(), n);
    arr->setFloat(c.G(), n+1);
    arr->setFloat(c.B(), n+2);
    arr->setFloat(c.A(), n+3);
}

}
