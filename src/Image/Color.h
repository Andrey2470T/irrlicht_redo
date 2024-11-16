#pragma once

#include <limits>

namespace img {

//! Class representing a color in RGBA format.
/** The color values can have an arbitrary type
 * (signed/unsigned 8-bit, 16-bit and 32-bit).
*/
template<T>
class ColorRGBA
{
public:
	//! Red component
	T R;
	//! Green component
	T G;
	//! Blue component
	T B;
	//! Alpha component
	T A;

	ColorRGB() : R(0), G(0), B(0), A(0)
	{}

	ColorRGB(T _R, T _G, T _B, T _A = 0) : R(_R), G(_G), B(_B), A(_A)
	{}

	//! Get lightness of the color
	f32 getLightness() const
	{
		return 0.5f * (std::max(std::max(R, G), B) + std::min(std::min(R, G), B));
	}

	//! Get luminance of the color
	f32 getLuminance() const
	{
		return 0.3f * R + 0.59f * G + 0.11f * B;
	}

	//! Get average intensity of the color
	u32 getAverage() const
	{
		return (R + G + B) / 3;
	}

	void operator==(const ColorRGBA<T> &other)
	{
		return (R == other.R && G == other.G && B == other.B && A == other.A);
	}

	void operator!=(const ColorRGBA<T> &other)
	{
		return !(*this == other);
	}

	void operator<(const ColorRGBA<T> &other)
	{
		return (R <= other.R && G <= other.G && B <= other.B && A <= other.A);
	}

	ColorRGB<T> operator+(const ColorRGBA<T> &other) const
	{
		return ColorRGB(
				std::min(R + other.R, std::numeric_limits<T>::max()),
				std::min(G + other.G, std::numeric_limits<T>::max()),
				std::min(B + other.B, std::numeric_limits<T>::max()));
	}

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
	\return Interpolated color. */
	ColorRGB<T> linInterp(const ColorRGBA<T> &other, f32 d) const
	{
		f32 clamped_d = std::clamp(d, 0.0f, 1.0f);
		return ColorRGBA<T>(
			lerp<T>(other.R, R, d),
			lerp<T>(other.G, G, d),
			lerp<T>(other.B, B, d),
			lerp<T>(other.A, A, d));
	}

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
	ColorRGB<T> quadInterp(const ColorRGB<T> &c1, const ColorRGB<T> &c2, f32 d) const
	{
		f32 clamped_d = std::clamp(d, 0.0f, 1.0f);
		return ColorRGB<T>(
			qerp<T>(R, c1.R, c2.R, d),
			qerp<T>(G, c1.G, c2.G, d),
			qerp<T>(B, c1.B, c2.B, d),
			qerp<T>(A, c1.A, c2.A, d),);
	}
};

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

	ColorHSL() : H(0.0f), S(0.0f), L(0.0f)
	{}

	ColorHSL(f32 h, f32 s, f32 l)
		: H(h), S(s), L(l)
	{}

	void fromRGBA(const ColorRGBA<f32> &color);
	void toRGBA(ColorRGBA<f32> &color) const;
private:
	inline f32 toRGBA1(f32 rm1, f32 rm2, f32 rh) const;
};

inline void ColorHSL::fromRGBA(const ColorRGBA<f32> &color)
{
	const f32 maxVal = std::max(color.R, color.G, color.B);
	const f32 minVal = std::min(color.R, color.G, color.B);

	L = (maxVal + minVal) * 50;

	if (equals<f32>(maxVal, minVal)) {
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

	if (equals<f32>(maxVal, color.R))
		H = (color.G - color.B) / delta;
	else if (equals<f32>(maxVal, color.G))
		H = 2 + ((color.B - color.R) / delta);
	else // blue is max
		H = 4 + ((color.R - color.G) / delta);

	H *= 60.0f;
	while (H < 0.0f)
		H += 360;
}

inline void ColorHSL::toRGBA(ColorRGBA<f32> &color) const
{
	const f32 l = L / 100;
	if (equals<f32>(S, 0.0f)) { // grey
		color.R = l;
		color.G = l;
		color.B = l;
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
	color.R = toRGBA1(rm1, rm2, h + 1.0f / 3.0f);
	color.G = toRGBA1(rm1, rm2, h);
	color.B = toRGBA1(rm1, rm2, h - 1.0f / 3.0f);
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

}

