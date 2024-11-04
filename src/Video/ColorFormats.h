#pragma once

#include <array>
#include "Common.h"

namespace render
{

enum ColorFormat
{
	CF_A1R5G5B5 = 0,
	CF_R5G6B5,
	CF_R8G8B8,
	CF_A8R8G8B8,
	CF_R16F,
	CF_G16R16F,
	CF_A16B16G16R16F,
	CF_R32F,
	CF_G32R32F,
	CF_A32B32G32R32F,
	CF_R8,
	CF_R8G8,
	CF_R16,
	CF_R16G16,
	CF_D16,
	CF_D32,
	CF_D24S8,
	CF_COUNT
};

struct ColorFormatInfo
{
	GLenum internalFormat;
	GLenum pixelFormat;
	GLenum pixelType;
	u32 size;
};

extern std::array<ColorFormatInfo, CF_COUNT> toGLFormatConverter;

//! calculate image data size in bytes for selected format, width and height.
u32 getDataSizeFromFormat(ColorFormat format, u32 width, u32 height);


template<T>
class ColorRGB
{
public:
	T R;
	T G;
	T B;

	ColorRGB() : R(0), G(0), B(0)
	{}

	ColorRGB(T _R, T _G, T _B) : R(_R), G(_G), B(_B)
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
	//! Get components as RGB array
	void getData(T *dest) const
	{
		*dest = R;
		*++dest = G;
		*++dest = B;
	}

	void operator==(const ColorRGB<T> &other)
	{
		return (R == other.R && G == other.G && B == other.B);
	}

	void operator!=(const ColorRGB<T> &other)
	{
		return !(*this == other);
	}

	void operator<(const ColorRGB<T> &other)
	{
		return (R < other.R && G < other.G && B < other.B);
	}

	ColorRGB<T> operator+(const ColorRGB<T> &other) const
	{
		return ColorRGB(
				std::min(R + other.R, T),
				std::min(G + other.G, T),
				std::min(B + other.B, T));
	}

	//! Interpolates the color with a f32 value to another color
	/** \param other: Other color
	\param d: value between 0.0f and 1.0f. d=0 returns other, d=1 returns this, values between interpolate.
	\return Interpolated color. */
	ColorRGB<T> linInterp(const ColorRGB<T> &other, f32 d) const
	{
		Vector3D<T> colorVec(R, G, B);
		Vector3D<T> otherColorVec(other.R, other.G, other.B);

		Vector3D<T> res = colorVec.linInterp(otherColorVec, d);

		return ColorRGB<T>(res.X, res.Y, res.Z);
	}

	//! Returns interpolated color. ( quadratic )
	/** \param c1: first color to interpolate with
	\param c2: second color to interpolate with
	\param d: value between 0.0f and 1.0f. */
	ColorRGB<T> quadInterp(const ColorRGB<T> &c1, const ColorRGB<T> &c2, f32 d) const
	{
		Vector3D<T> colorVec(R, G, B);
		Vector3D<T> colorVec1(c1.R, c1.G, c1.B);
		Vector3D<T> colorVec2(c2.R, c2.G, c2.B);

		Vector3D<T> res = colorVec.quadInterp(colorVec1, colorVec2, d);

		return ColorRGB<T>(res.X, res.Y, res.Z);
	}
}

}

