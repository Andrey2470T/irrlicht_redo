#pragma once

#include "Image/Image.h"
#include "Image/ImageModifier.h"

namespace render
{

enum TextureType
{
    TT_2D = 0,
    TT_CUBEMAP
};

enum CubeMapFace
{
	CMF_POS_X = 0,
	CMF_NEG_X,
	CMF_POS_Y,
	CMF_NEG_Y,
	CMF_POS_Z,
	CMF_NEG_Z,
	CMF_COUNT
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
	u32 texID;
	std::string name;

	u32 width;
	u32 height;

	img::PixelFormat format;

	TextureSettings texSettings;
public:
	Texture(const std::string &_name, u32 _width, u32 _height, img::PixelFormat _format,
			const TextureSettings &_texSettings=TextureSettings());
	
    virtual ~Texture();

	std::string getName() const
	{
		return name;
	}

	virtual TextureType getType() const = 0;
	
	u32 getID() const
	{
		return texID;
	}

	u32 getWidth() const
	{
		return width;
	}

	u32 getHeight() const
	{
		return height;
	}

    img::PixelFormat getFormat() const
	{
		return format;
	}

	bool hasMipMaps() const
	{
		return texSettings.hasMipMaps;
	}
	bool isRenderTarget() const
	{
		return texSettings.isRenderTarget;
	}

	virtual void bind() const = 0;
	virtual void unbind() const = 0;

	virtual void uploadData(img::Image *img, img::ImageModifier *imgMod = nullptr) = 0;
	virtual void uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod = nullptr) = 0;

    virtual img::Image *downloadData() const = 0;
	virtual void regenerateMipMaps(u8 max_level) = 0;
	
    bool operator==(const Texture &other)
	{
        return texID == other.texID;
	}
	
    bool operator!=(const Texture &other)
	{
        return texID != other.texID;
	}
};

}
