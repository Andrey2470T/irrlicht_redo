#pragma once

#include "Utils/ByteArray.h"
#include "PixelFormats.h"
#include "Utils/MathFuncs.h"

namespace img {

//! Class representing a color in RGBA format.
/** The color values can have an arbitrary type
 * (signed/unsigned 8-bit, 16-bit and 32-bit).
*/

bool isFormatSupportedForColor8(PixelFormat format);

class color8
{
	PixelFormat format;

	//! Color represented as a byte array saving the color data
	//! depending on the format
    ByteArray color;
public:
    color8();

    color8(PixelFormat _format);

    color8(PixelFormat _format, u8 _R, u8 _G = 0, u8 _B = 0, u8 _A = 0);

    color8(PixelFormat _format, u32 colorNum);

    color8(const color8 &other);

    color8 &operator=(const color8 &other);

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
    f32 getLightness() const;

	//! Get luminance of the color
    f32 getLuminance() const;

	//! Get average intensity of the color
    u32 getAverage() const;

    void *data()
    {
        return color.data();
    }
    const void *data() const
    {
        return color.data();
    }

    bool operator==(const color8 &other) const;

    bool operator!=(const color8 &other) const;

    bool operator<(const color8 &other) const;

    color8 operator+(const color8 &other) const;

    color8 &operator+=(const color8 &other);

    color8 operator+(u8 val) const;

    color8 &operator+=(u8 val);

    color8 operator-(const color8 &other) const;

    color8 &operator-=(const color8 &other);

    color8 operator-(u8 val) const;

    color8 &operator-=(u8 val);

    color8 operator*(const color8 &other) const;

    color8 &operator*=(const color8 &other);

    color8 operator*(u8 val) const;

    color8 &operator*=(u8 val);

    color8 operator/(const color8 &other) const;

    color8 &operator/=(const color8 &other);

    color8 operator/(u8 val) const;

    color8 &operator/=(u8 val);

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
	\return Interpolated color. */
    color8 linInterp(const color8 &other, f32 d) const;

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
    color8 quadInterp(const color8 &c1, const color8 &c2, f32 d) const;

    // res = c1 * d + c2 * (1- d)
    static color8 linInterp(const color8 &c1, const color8 &c2, f32 d);
private:
    void set(u8 R, u8 G, u8 B, u8 A);

    u8 getChannel(u32 n) const;

    void setChannel(u8 v, u32 n);
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
    colorf linInterp(const colorf &other, f32 d) const;

    //! Returns interpolated color. ( quadratic )
    /** \param c1: first color to interpolate with
    \param c2: second color to interpolate with
    \param d: value between 0.0f and 1.0f. */
    colorf quadInterp(const colorf &c1, const colorf &c2, f32 d) const;

    // res = c1 * d + c2 * (1- d)
    static colorf linInterp(const colorf &c1, const colorf &c2, f32 d);
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

}

