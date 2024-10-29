#pragma once

#include <string>
#include <memory>


enum TextureType
{
	TT_2D = 0,
	TT_CUBEMAP
};

enum TextureFormat
{
	TF_A1R5G5B5 = 0,
	TF_R5G6B5,
	TF_R8G8B8,
	TF_A8R8G8B8,
	TF_R16F,
	TF_G16R16F,
	TF_A16B16G16R16F,
	TF_R32F,
	TF_G32R32F,
	TF_A32B32G32R32F,
	TF_R8,
	TF_R8G8,
	TF_R16,
	TF_R16G16,
	TF_D16,
	TF_D32,
	TF_D24S8
};

enum TextureWrapping
{
	TW_REPEAT = 0,
	TW_CLAMP,
	TW_CLAMP_TO_EDGE,
	TW_CLAMP_TO_BORDER,
	TW_MIRROR,
	TW_MIRROR_CLAMP,
	TW_MIRROR_CLAMP_TO_EDGE,
	TW_MIRROR_CLAMP_TO_BORDER
};

enum TextureMinFilter
{
	TMF_NEAREST_MIPMAP_NEAREST = 0,
	TMF_LINEAR_MIPMAP_NEAREST,
	TMF_NEAREST_MIPMAP_LINEAR,
	TMF_LINEAR_MIPMAP_LINEAR
};

enum TextureMagFilter
{
	TMAGF_NEAREST = 0,
	TMAGF_LINEAR
};

struct TextureSettings
{
	TextureWrapping wrapU;
	TextureWrapping wrapV;
	TextureWrapping wrapW;

	TextureMinFilter minF;
	TextureMagFilter maxF;

	bool hasMipMaps;
	u8 maxMipLevel;
};


class Texture
{
	std::string name;

	u32 width;
	u32 height;

	TextureFormat format;
	TextureSettings settings;

	std::shared_ptr<Image> imgCache;
public:
	Texture(std::string name, u32 _width, u32 _height, TextureFormat _format)
		: width(_width), height(_height), format(_format)
	{}

	Texture(const std::shared_ptr<Image> image);

	std::string getName() const
	{
		return name;
	}

	TextureType getType() const = 0;

	u32 getWidth() const
	{
		return width;
	}

	u32 getHeight() const
	{
		return height;
	}

	TextureFormat getFormat() const
	{
		return format;
	}

	bool hasMipMaps() const
	{
		return settings.hasMipMaps;
	}

	void bind() const = 0;
	void unbind() const = 0;

	Image *downloadData() const = 0;
	void regenerateMipMaps(u8 max_level) = 0;
};


