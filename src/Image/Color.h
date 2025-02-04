#pragma once

#include "Utils/ByteArray.h"
#include "PixelFormats.h"
#include "Utils/MathFuncs.h"
#include <optional>

namespace img {

//! Class representing a color in RGBA format.
/** The color values can have an arbitrary type
 * (signed/unsigned 8-bit, 16-bit and 32-bit).
*/

template <class T>
class ColorRGBA
{
	PixelFormat format;

	//! Color represented as a byte array saving the color data
	//! depending on the format
	utils::ByteArray color;
public:
    ColorRGBA() = default;
	ColorRGBA(PixelFormat _format)
		: format(_format), color(pixelFormatInfo[format].size / 8)
	{}

	ColorRGBA(PixelFormat _format, T _R, T _G = 0, T _B = 0, T _A = 0)
		: format(_format), color(pixelFormatInfo[format].size / 8)
	{
		set(_R, _G, _B, _A);
	}

	ColorRGBA(const ColorRGBA &other)
		: format(other.format), color(other.color) {}

	ColorRGBA<T> &operator=(const ColorRGBA<T> &other)
	{
		set(other.R(), other.G(), other.B(), other.A());

		return *this;
	}

	T R() const { return getChannel(0); }
	T G() const { return getChannel(1); }
	T B() const { return getChannel(2); }
	T A() const { return getChannel(3); }

	void R(T R) { return setChannel(R, 0); }
	void G(T G) { return setChannel(G, 1); }
	void B(T B) { return setChannel(B, 2); }
	void A(T A) { return setChannel(A, 3); }

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
			return 0.5f * (utils::max3(R(), G(), B()) + utils::min3(R(), G(), B()));
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
			return (R() + G()) / 2;
		else
			return (R() + G() + B()) / 3;
	}

	bool operator==(const ColorRGBA<T> &other) const;

	bool operator!=(const ColorRGBA<T> &other) const;

	bool operator<(const ColorRGBA<T> &other) const;

	ColorRGBA<T> operator+(const ColorRGBA<T> &other) const;

	ColorRGBA<T> &operator+=(const ColorRGBA<T> &other);

	ColorRGBA<T> operator+(T val) const;

	ColorRGBA<T> &operator+=(T val);

	ColorRGBA<T> operator-(const ColorRGBA<T> &other) const;

	ColorRGBA<T> &operator-=(const ColorRGBA<T> &other);

	ColorRGBA<T> operator-(T val) const;

	ColorRGBA<T> &operator-=(T val);

	ColorRGBA<T> operator*(const ColorRGBA<T> &other) const;

	ColorRGBA<T> &operator*=(const ColorRGBA<T> &other);

	ColorRGBA<T> operator*(T val) const;

	ColorRGBA<T> &operator*=(T val);

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
	\return Interpolated color. */
	ColorRGBA<T> linInterp(const ColorRGBA<T> &other, f32 d) const;

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
	ColorRGBA<T> quadInterp(const ColorRGBA<T> &c1, const ColorRGBA<T> &c2, f32 d) const;
private:
	void set(T R, T G, T B, T A);

	T getChannel(u32 n) const;

	void setChannel(T v, s32 n=-1);
};

typedef ColorRGBA<u8> color8;
typedef ColorRGBA<f32> colorf;

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

	inline void fromRGBA(const ColorRGBA<f32> &color);
	inline void toRGBA(ColorRGBA<f32> &color) const;
private:
	inline f32 toRGBA1(f32 rm1, f32 rm2, f32 rh) const;
};

color8 getColor8(ByteArray *arr, u32 n);
void setColor8(ByteArray *arr, color8 c, std::optional<u32> n);

}

