#include "Color.h"

namespace img
{

template <class T>
inline bool ColorRGBA<T>::operator==(const ColorRGBA<T> &other) const
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

template <class T>
inline bool ColorRGBA<T>::operator!=(const ColorRGBA<T> &other) const
{
	return !(*this == other);
}

template <class T>
inline bool ColorRGBA<T>::operator<(const ColorRGBA<T> &other) const
{
	u8 channelsCount = pixelFormatInfo[format].channels;

	if (channelsCount == 1)
		return R() <= other.R();
	else if (channelsCount == 2)
		return (R() <= other.R && G() <= other.G());
	else if (channelsCount == 3)
		return (R() <= other.R() && G() <= other.G() && B() <= other.B());
	else
		return (R() <= other.R() && G() <= other.G() && B() <= other.B() && A() <= other.A());
}

template <class T>
inline ColorRGBA<T> ColorRGBA<T>::operator+(const ColorRGBA<T> &other) const
{
	return ColorRGBA<T>(
		getFormat(),
		R() + other.R(),
		G() + other.G(),
		B() + other.B(),
		A() + other.A());
}

template <class T>
inline ColorRGBA<T> &ColorRGBA<T>::operator+=(const ColorRGBA<T> &other)
{
	R(R() + other.R());
	G(G() + other.G());
	B(B() + other.B());
	A(A() + other.A());

	return *this;
}

template <class T>
inline ColorRGBA<T> ColorRGBA<T>::operator+(T val) const
{
	return ColorRGBA<T>(
		getFormat(),
		R() + val,
		G() + val,
		B() + val,
		A() + val);
}

template <class T>
inline ColorRGBA<T> &ColorRGBA<T>::operator+=(T val)
{
	R(R() + val);
	G(G() + val);
	B(B() + val);
	A(A() + val);

	return *this;
}

template <class T>
inline ColorRGBA<T> ColorRGBA<T>::operator-(const ColorRGBA<T> &other) const
{
	return ColorRGBA<T>(
		getFormat(),
		R() - other.R(),
		G() - other.G(),
		B() - other.B(),
		A() - other.A());
}

template <class T>
inline ColorRGBA<T> &ColorRGBA<T>::operator-=(const ColorRGBA<T> &other)
{
	R(R() - other.R());
	G(G() - other.G());
	B(B() - other.B());
	A(A() - other.A());

	return *this;
}

template <class T>
inline ColorRGBA<T> ColorRGBA<T>::operator-(T val) const
{
	return ColorRGBA<T>(
		getFormat(),
		R() - val,
		G() - val,
		B() - val,
		A() - val);
}

template <class T>
inline ColorRGBA<T> &ColorRGBA<T>::operator-=(T val)
{
	R(R() - val);
	G(G() - val);
	B(B() - val);
	A(A() - val);

	return *this;
}

template <class T>
inline ColorRGBA<T> ColorRGBA<T>::operator*(const ColorRGBA<T> &other) const
{
	return ColorRGBA<T>(
		getFormat(),
		R() * other.R(),
		G() * other.G(),
		B() * other.B(),
		A() * other.A());
}

template <class T>
inline ColorRGBA<T> &ColorRGBA<T>::operator*=(const ColorRGBA<T> &other)
{
	R(R() * other.R());
	G(G() * other.G());
	B(B() * other.B());
	B(A() * other.A());

	return *this;
}

template <class T>
inline ColorRGBA<T> ColorRGBA<T>::operator*(T val) const
{
	return ColorRGBA<T>(
		getFormat(),
		R() * val,
		G() * val,
		B() * val,
		A() * val);
}

template <class T>
inline ColorRGBA<T> &ColorRGBA<T>::operator*=(T val)
{
	R(R() * val);
	G(G() * val);
	B(B() * val);
	A(A() * val);

	return *this;
}

//! Interpolates the color with a f32 value to another color
/** \param other: Other color
\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
\return Interpolated color. */
template <class T>
inline ColorRGBA<T> ColorRGBA<T>::linInterp(const ColorRGBA<T> &other, f32 d) const
{
	return ColorRGBA<T>(
		getFormat(),
		utils::lerp<T>(other.R(), R(), d),
		utils::lerp<T>(other.G(), G(), d),
		utils::lerp<T>(other.B(), B(), d),
		A());
}

//! Returns interpolated color. ( quadratic )
/** \param c1: first color to interpolate with
\param c2: second color to interpolate with
\param d: value between 0.0f and 1.0f. */
template <class T>
inline ColorRGBA<T> ColorRGBA<T>::quadInterp(const ColorRGBA<T> &c1, const ColorRGBA<T> &c2, f32 d) const
{
	return ColorRGBA<T>(
		getFormat(),
		utils::qerp<T>(R(), c1.R(), c2.R(), d),
		utils::qerp<T>(G(), c1.G(), c2.G(), d),
		utils::qerp<T>(B(), c1.B(), c2.B(), d),
		A());
}

template <class T>
inline void ColorRGBA<T>::set(T R, T G, T B, T A)
{
	u8 channelsCount = pixelFormatInfo[format].channels;

	setChannel(R);

	if (channelsCount > 1)
		setChannel(G);
	if (channelsCount > 2)
		setChannel(B);
	if (channelsCount > 3)
		setChannel(A);
}

template <class T>
inline T ColorRGBA<T>::getChannel(u32 n) const
{
	BasicType type = pixelFormatInfo[format].type;

	switch (type) {
		case BasicType::UINT8:
			return color.getUInt8(n);
		case BasicType::UINT16:
			return color.getUInt16(n);
		case BasicType::UINT32:
			return color.getUInt32(n);
		case BasicType::FLOAT:
			return color.getFloat(n);
		default:
			return 0;
	};
}

template <class T>
inline void ColorRGBA<T>::setChannel(T v, s32 n)
{
	BasicType type = pixelFormatInfo[format].type;

	switch (type) {
		case BasicType::UINT8:
			color.setUInt8(v, n);
			break;
		case BasicType::UINT16:
			color.setUInt16(v, n);
			break;
		case BasicType::UINT32:
			color.setUInt32(v, n);
			break;
		case BasicType::FLOAT:
			color.setFloat(v, n);
			break;
		default:
			break;
	};
}


inline void ColorHSL::fromRGBA(const ColorRGBA<f32> &color)
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

inline void ColorHSL::toRGBA(ColorRGBA<f32> &color) const
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

color8 getColor8(ByteArray *arr, u32 n)
{
	color8 c(PF_RGBA8);

	c.R(arr->getUInt8(n));
	c.G(arr->getUInt8(n+1));
	c.B(arr->getUInt8(n+2));
	c.A(arr->getUInt8(n+3));

	return c;
}

void setColor8(ByteArray *arr, color8 c, std::optional<u32> n)
{
	arr->setUInt8(c.R(), n);
	arr->setUInt8(c.G(), n ? n.value()+1 : n);
	arr->setUInt8(c.B(), n ? n.value()+2 : n);
	arr->setUInt8(c.A(), n ? n.value()+3 : n);
}

}
