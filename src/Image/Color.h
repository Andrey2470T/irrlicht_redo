#pragma once

#include <limits>

namespace img {

template<class T>
ColorRGBA<T> clampColor(const ColorRGBA &c)
{
	return ColorRGBA<T>(
		utils::limClamp<T>(c.R),
		utils::limClamp<T>(c.G),
		utils::limClamp<T>(c.B),
		c.A);
}

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
	std::vector<u8> color;
public:
	ColorRGBA(PixelFormat _format)
		: format(_format)
	{
		color.resize(pixelFormatInfo[format].size / 8);
	}

	ColorRGBA(PixelFormat _format, T _R, T _G = 0, T _B = 0, T _A = 0)
		: format(_format)
	{
		set(R, G, B, A);
	}

	ColorRGBA(const ColorRGBA &other)
		: format(other.format), color(other.color) {}

	T R() const { return getChannel('R'); }
	T G() const { return getChannel('G'); }
	T B() const { return getChannel('B'); }
	T A() const { return getChannel('A'); }

	void R(T R) { return setChannel(R, 'R'); }
	void G(T G) { return setChannel(G, 'G'); }
	void B(T B) { return setChannel(B, 'B'); }
	void A(T A) { return setChannel(A, 'A'); }

	//! Get lightness of the color
	f32 getLightness() const
	{
		u8 channelsCount = getChannelsCount(format);

		if (channelsCount == 1)
			return 0.5f * R();
		else if (channelsCount == 2)
			return 0.5f * (std::max(R(), G()) + std::min(R(), G()));
		else
			return 0.5f * (utils::max3(R(), G(), B()) + utils::min3(R, G, B));
	}

	//! Get luminance of the color
	f32 getLuminance() const
	{
		if (getChannelsCount < 3)
			return 0.0f;
		return 0.3f * R + 0.59f * G + 0.11f * B;
	}

	//! Get average intensity of the color
	u32 getAverage() const
	{
		u8 channelsCount = getChannelsCount(format);

		if (channelsCount == 1)
			return R();
		else if (channelsCount == 2)
			return (R + G) / 2;
		else
			return (R + G + B) / 3;
	}

	void operator==(const ColorRGBA<T> &other)
	{
		u8 channelsCount = getChannelsCount(format);

		if (channelsCount == 1)
			return R() == other.R();
		else if (channelsCount == 2)
			return (R() == other.R() && G() == other.G());
		else if (channelsCount == 3)
			return (R() == other.R() && G() == other.G() && B == other.B());
		else
			return (R() == other.R() && G() == other.G() && B() == other.B() && A() == other.A());
	}

	void operator!=(const ColorRGBA<T> &other)
	{
		return !(*this == other);
	}

	void operator<(const ColorRGBA<T> &other)
	{
		u8 channelsCount = getChannelsCount(format);

		if (channelsCount == 1)
			return R() <= other.R();
		else if (channelsCount == 2)
			return (R() <= other.R && G() <= other.G());
		else if (channelsCount == 3)
			return (R() <= other.R() && G() <= other.G() && B() <= other.B());
		else
			return (R() <= other.R() && G() <= other.G() && B() <= other.B() && A() <= other.A());
	}

	ColorRGBA<T> operator+(const ColorRGBA<T> &other) const
	{
		return ColorRGBA<T>(
			limClamp<T>(R() + other.R()),
			limClamp<T>(G() + other.G()),
			limClamp<T>(B() + other.B()),
			limClamp<T>(A() + other.A()))
	}

	ColorRGBA<T> &operator+=(const ColorRGBA<T> &other) const
	{
		R(limClamp<T>(R() + other.R));
		G(limClamp<T>(G() + other.G));
		B(limClamp<T>(B() + other.B));
		A(limClamp<T>(A() + other.A));

		return *this;
	}

	ColorRGBA<T> operator+(T val) const
	{
		return ColorRGBA<T>(
			limClamp<T>(R() + val),
			limClamp<T>(G() + val),
			limClamp<T>(B() + val),
			limClamp<T>(A() + val));
	}

	ColorRGBA<T> &operator+=(T val) const
	{
		R(limClamp<T>(R() + val));
		G(limClamp<T>(G() + val));
		B(limClamp<T>(B) + val));
		A(limClamp<T>(A() + val));

		return *this;
	}

	ColorRGBA<T> operator*(const ColorRGBA<T> &other) const
	{
		return ColorRGBA<T>(
			limClamp<T>(R() * other.R()),
			limClamp<T>(G() * other.G()),
			limClamp<T>(B() * other.B()),
			limClamp<T>(A() + other.A()));
	}

	ColorRGBA<T> &operator*=(const ColorRGBA<T> &other) const
	{
		R(limClamp<T>(R() * other.R()));
		G(limClamp<T>(G() * other.G()));
		B(limClamp<T>(B() * other.B()));
		B(limClamp<T>(A() * other.A()));

		return *this;
	}

	ColorRGBA<T> operator*(T val) const
	{
		return ColorRGBA<T>(
			limClamp<T>(R() * val),
			limClamp<T>(G() * val),
			limClamp<T>(B() * val),
			limClamp<T>(A() * val));
	}

	ColorRGBA<T> &operator*=(T val) const
	{
		R(limClamp<T>(R() * val));
		G(limClamp<T>(G() * val));
		B(limClamp<T>(B() * val));
		A(limClamp<T>(A() * val));

		return *this;
	}

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
	\return Interpolated color. */
	ColorRGBA<T> linInterp(const ColorRGBA<T> &other, f32 d) const
	{
		return ColorRGBA<T>(
			lerp<T>(other.R(), R(), d),
			lerp<T>(other.G(), G(), d),
			lerp<T>(other.B(), B(), d),
			A());
	}

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
	ColorRGBA<T> quadInterp(const ColorRGBA<T> &c1, const ColorRGBA<T> &c2, f32 d) const
	{
		return ColorRGBA<T>(
			qerp<T>(R(), c1.R(), c2.R(), d),
			qerp<T>(G(), c1.G(), c2.G(), d),
			qerp<T>(B(), c1.B(), c2.B(), d),
			A());
	}
private:
	void set(T R, T G, T B, T A)
	{
		u8 channelsCount = pixelFormatInfo[format].channels;

		setChannel(R, 'R');

		if (channelsCount > 1)
			setChannel(G, 'G');
		if (channelsCount > 2)
			setChannel(B, 'B');
		if (channelsCount > 3)
			setChannel(A, 'A');
	}

	void setChannel(T v, char type)
	{
		auto &pixel_info = pixelFormatInfo[format];
		u32 channelBytesSize = (pixel_info.size / pixel_info.channels) / 8;

		u32 offset = 0;

		if (type == 'G') {
			if (channelsCount < 2)
				return;
			offset += channelBytesSize;
		}
		else if (type == 'B') {
			if (channelsCount < 3)
				return;
			offset += channelBytesSize*2;
		}
		else if (type == 'A') {
			if (channelsCount < 4)
				return;
			offset += channelBytesSize*3;
		}

		if (channelBytesSize == 1) {
			u8 ch = (u8)v;
			color[offset] = ch;
		}
		else if (channelSize == 2) {
			u16 ch = *(u16*)&v;

			color[offset] = ch >> 8;
			color[offset+1] = ch & 0xFF;
		}
		else if (channelSize == 4) {
			u32 ch = *(u32*)&v;

			color[offset] = ch >> 24;
			color[offset+1] = ch >> 16 & 0xFF;
			color[offset+2] = ch >> 8 & 0xFF;
			color[offset+3] = ch & 0xFF;
		}
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

	ColorHSL() : H(0.0f), S(0.0f), L(0.0f) {}

	ColorHSL(f32 h, f32 s, f32 l)
		: H(h), S(s), L(l) {}

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

typedef ColorRGBA<u8> color8;
typedef ColorRGBA<f32> colorf;

}

