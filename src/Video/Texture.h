#pragma once

#include <string>
#include "TextureFormats.h"
#include "toGLEnum.h"


namespace render
{

enum TextureType
{
	TT_2D = 0;
	TT_CUBEMAP;
};

struct TextureSettings
{
	TextureWrapping wrapU = TW_REPEAT;
	TextureWrapping wrapV = TW_REPEAT;
	TextureWrapping wrapW = TW_REPEAT;

	TextureMinFilter minF = TMF_NEAREST;
	TextureMagFilter magF = TMAGF_NEAREST;

	bool hasMipMaps;
	u8 maxMipLevel;
};


class Texture
{
protected:
	std::string name;

	u32 width;
	u32 height;

	TextureFormat format;
	TextureFormatInfo format_info;
	
	bool isRenderTarget;
public:
	Texture(const std::string &_name, u32 _width, u32 _height, TextureFormat _format)
		: name(_name), width(_width), height(_height), format(_format),
		  format_info(toGLFormatConverter.at(_format))
	{}

	std::string getName() const
	{
		return name;
	}

	virtual TextureType getType() const = 0;

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

	virtual bool hasMipMaps() const;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual Image *downloadData() const = 0;
	virtual void regenerateMipMaps(u8 max_level) = 0;
};

}
