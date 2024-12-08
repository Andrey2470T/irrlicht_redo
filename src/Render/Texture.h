#pragma once

#include "Common.h"
#include "Image/Image.h"
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

	bool isRenderTarget;
	bool hasMipMaps;
	u8 maxMipLevel;
};


class Texture
{
protected:
	std::string name;

	u32 width;
	u32 height;

	img::PixelFormat format;
public:
	Texture(const std::string &_name, u32 _width, u32 _height, img::PixelFormat _format)
		: name(_name), width(_width), height(_height), format(_format)
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

	img::TextureFormat getFormat() const
	{
		return format;
	}

	virtual bool hasMipMaps() const;
	virtual bool isRenderTarget() const;

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual void uploadData(void *data);
	virtual void uploadSubData(u32 x, u32 y, void *data);

	virtual std::unique_ptr<img::Image> downloadData() const = 0;
	virtual void regenerateMipMaps(u8 max_level) = 0;
};

}
