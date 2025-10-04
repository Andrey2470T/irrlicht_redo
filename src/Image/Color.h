#pragma once

#include "Utils/ByteArray.h"
#include "PixelFormats.h"
#include "Utils/MathFuncs.h"

namespace img {

//! Class representing a color in RGBA format.
/** The color values can have an arbitrary type
 * (signed/unsigned 8-bit, 16-bit and 32-bit).
*/

bool isFormatSupportedForImage(PixelFormat format);

u8 addCh(u8 ch1, u8 ch2);
u8 subCh(u8 ch1, u8 ch2);
u8 mulCh(u8 ch1, u8 ch2);
u8 divCh(u8 ch1, u8 ch2);

class color8
{
	PixelFormat format;

	//! Color represented as a byte array saving the color data
	//! depending on the format
    ByteArray color;
public:
    color8() = default;
    color8(PixelFormat _format)
        : format(_format), color(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8)
    {
        assert(isFormatSupportedForImage(format));
        set(0, 0, 0, 0);
    }

    color8(PixelFormat _format, u8 _R, u8 _G = 0, u8 _B = 0, u8 _A = 0)
        : format(_format), color(pixelFormatInfo[format].channels, pixelFormatInfo[format].size / 8)
	{
        assert(isFormatSupportedForImage(format));
		set(_R, _G, _B, _A);
	}

    color8(const color8 &other)
        : format(other.format), color(other.color)
    {
        assert(isFormatSupportedForImage(format));
    }

    color8 &operator=(const color8 &other)
	{
		format = other.getFormat();
        assert(isFormatSupportedForImage(format));

		set(other.R(), other.G(), other.B(), other.A());

		return *this;
	}

    u8 R() const { return getChannel(0); }
    u8 G() const { return getChannel(1); }
    u8 B() const { return getChannel(2); }
    u8 A() const { return getChannel(3); }

    void R(u8 R) { return setChannel(R, 0); }
    void G(u8 G) { return setChannel(G, 1); }
    void B(u8 B) { return setChannel(B, 2); }
    void A(u8 A) { return setChannel(A, 3); }

	PixelFormat getFormat() const { return format; }

	//! Get lightness of the color
	f32 getLightness() const
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
	f32 getLuminance() const
	{
		if (pixelFormatInfo[format].channels < 3)
			return 0.0f;
		return 0.3f * R() + 0.59f * G() + 0.11f * B();
	}

	//! Get average intensity of the color
	u32 getAverage() const
	{
		u8 channelsCount = pixelFormatInfo[format].channels;

		if (channelsCount == 1)
			return R();
		else if (channelsCount == 2)
            return addCh(R(), G()) / 2;
		else
            return addCh(addCh(R(), G()), B()) / 3;
	}

    bool operator==(const color8 &other) const
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

    bool operator!=(const color8 &other) const
	{
		return !(*this == other);
	}

    bool operator<(const color8 &other) const
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

    color8 operator+(const color8 &other) const
	{
        return color8(
			getFormat(),
            addCh(R(), other.R()),
            addCh(G(), other.G()),
            addCh(B(), other.B()),
            other.A());
	}

    color8 &operator+=(const color8 &other)
	{
        R(addCh(R(), other.R()));
        G(addCh(G(), other.G()));
        B(addCh(B(), other.B()));
        A(other.A());

		return *this;
	}

    color8 operator+(u8 val) const
	{
        return color8(
			getFormat(),
            addCh(R(), val),
            addCh(G(), val),
            addCh(B(), val),
            A());
	}

    color8 &operator+=(u8 val)
	{
        R(addCh(R(), val));
        G(addCh(G(), val));
        B(addCh(B(), val));

		return *this;
	}

    color8 operator-(const color8 &other) const
	{
        return color8(
			getFormat(),
            subCh(R(), other.R()),
            subCh(G(), other.G()),
            subCh(B(), other.B()),
            other.A());
	}

    color8 &operator-=(const color8 &other)
	{
        R(subCh(R(), other.R()));
        G(subCh(G(), other.G()));
        B(subCh(B(), other.B()));
        A(other.A());

		return *this;
	}

    color8 operator-(u8 val) const
	{
        return color8(
			getFormat(),
            subCh(R(), val),
            subCh(G(), val),
            subCh(B(), val),
            A());
	}

    color8 &operator-=(u8 val)
	{
        R(subCh(R(), val));
        G(subCh(G(), val));
        B(subCh(B(), val));

		return *this;
	}

    color8 operator*(const color8 &other) const
	{
        return color8(
			getFormat(),
            mulCh(R(), other.R()),
            mulCh(G(), other.G()),
            mulCh(B(), other.B()),
            other.A());
	}

    color8 &operator*=(const color8 &other)
	{
        R(mulCh(R(), other.R()));
        G(mulCh(G(), other.G()));
        B(mulCh(B(), other.B()));
        B(other.A());

		return *this;
	}

    color8 operator*(u8 val) const
	{
        return color8(
			getFormat(),
            mulCh(R(), val),
            mulCh(G(), val),
            mulCh(B(), val),
            A());
	}

    color8 &operator*=(u8 val)
	{
        R(mulCh(R(), val));
        G(mulCh(G(), val));
        B(mulCh(B(), val));

		return *this;
	}

    color8 operator/(const color8 &other) const
    {
        return color8(
            getFormat(),
            divCh(R(), other.R()),
            divCh(G(), other.G()),
            divCh(B(), other.B()),
            other.A());
    }

    color8 &operator/=(const color8 &other)
    {
        R(divCh(R(), other.R()));
        G(divCh(G(), other.G()));
        B(divCh(B(), other.B()));
        B(other.A());

        return *this;
    }

    color8 operator/(u8 val) const
    {
        return color8(
            getFormat(),
            divCh(R(), val),
            divCh(G(), val),
            divCh(B(), val),
            A());
    }

    color8 &operator/=(u8 val)
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
    color8 linInterp(const color8 &other, f32 d) const
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
    color8 quadInterp(const color8 &c1, const color8 &c2, f32 d) const
	{
        return color8(
			getFormat(),
            qerp<u8>(R(), c1.R(), c2.R(), d),
            qerp<u8>(G(), c1.G(), c2.G(), d),
            qerp<u8>(B(), c1.B(), c2.B(), d),
			A());
	}

    // res = c1 * d + c2 * (1- d)
    static color8 linInterp(const color8 &c1, const color8 &c2, f32 d)
    {
        return color8(
            c1.getFormat(),
            lerp<u8>(c2.R(), c1.R(), d),
            lerp<u8>(c2.G(), c1.G(), d),
            lerp<u8>(c2.B(), c1.B(), d),
            c1.A());
    }
private:
    void set(u8 R, u8 G, u8 B, u8 A)
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

    u8 getChannel(u32 n) const
	{
        auto channelsCount = pixelFormatInfo[format].channels;

        if (n >= channelsCount)
            return 0;
        \
        return color.getUInt8(n);
	}

    void setChannel(u8 v, u32 n)
	{
        auto channelsCount = pixelFormatInfo[format].channels;

        if (n >= channelsCount)
            return;

        color.setUInt8(v, n);
	}
};

class colorf
{
    f32 r, g, b, a;
public:
    colorf()
        : r(0.0f), g(0.0f), b(0.0f), a(1.0f)
    {}

    colorf(f32 _r, f32 _g, f32 _b, f32 _a = 1.0f)
        : r(_r), g(_g), b(_b), a(_a)
    {}

    f32 R() const { return r; }
    f32 G() const { return g; }
    f32 B() const { return b; }
    f32 A() const { return a; }

    void R(f32 _r) { r = _r; }
    void G(f32 _g) { g = _g; }
    void B(f32 _b) { b = _b; }
    void A(f32 _a) { a = _a; }

    bool operator==(const colorf &other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(const colorf &other) const
    {
        return !(*this == other);
    }

    //! Interpolates the color with a f32 value to another color
    /** \param other: Other color
    \param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
    \return Interpolated color. */
    colorf linInterp(const colorf &other, f32 d) const
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
    colorf quadInterp(const colorf &c1, const colorf &c2, f32 d) const
    {
        return colorf(
            qerp<f32>(R(), c1.R(), c2.R(), d),
            qerp<f32>(G(), c1.G(), c2.G(), d),
            qerp<f32>(B(), c1.B(), c2.B(), d),
            A());
    }

    // res = c1 * d + c2 * (1- d)
    static colorf linInterp(const colorf &c1, const colorf &c2, f32 d)
    {
        return colorf(
            lerp<f32>(c2.R(), c1.R(), d),
            lerp<f32>(c2.G(), c1.G(), d),
            lerp<f32>(c2.B(), c1.B(), d),
            c1.A());
    }
};


// Named (often used) colors
extern color8 white;
extern color8 black;
extern color8 gray;
extern color8 red;
extern color8 green;
extern color8 blue;
extern color8 yellow;
extern color8 violet;
extern color8 cyan;
extern color8 orange;
extern color8 pink;
extern color8 brown;

//! Class representing a color in HSL format
/** The color values for hue, saturation, luminance
are stored in 32bit floating point variables. Hue is in range [0,360],
Luminance and Saturation are in percent [0,100]
*/
class ColorHSL
{
public:
	//! Hue component
	f32 H;
	//! Saturation component
	f32 S;
	//! Luminance component
	f32 L;

	ColorHSL() : H(0.0f), S(0.0f), L(0.0f) {}

	ColorHSL(f32 h, f32 s, f32 l)
		: H(h), S(s), L(l) {}

    void fromRGBA(const colorf &color);
    void toRGBA(colorf &color) const;
private:
	inline f32 toRGBA1(f32 rm1, f32 rm2, f32 rh) const;
};

color8 getColor8(const ByteArray *arr, u32 n, img::PixelFormat format=img::PF_RGBA8);
void setColor8(ByteArray *arr, const color8 &c, u32 n, img::PixelFormat format=img::PF_RGBA8);

}

