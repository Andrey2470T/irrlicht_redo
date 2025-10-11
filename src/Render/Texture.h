#pragma once

#include "Image/Image.h"
#include "Image/ImageModifier.h"

namespace render
{

enum TextureType : u8
{
    TT_2D = 0,
    TT_CUBEMAP
};

enum CubeMapFace : u8
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

    f32 lodBias;
    u8 anisotropyFilter = 0;

    bool isRenderTarget = true;
    bool hasMipMaps = false;
    u8 maxMipLevel = 0;

    f32 maxLodBias = 0.0f;
    u8 maxAnisotropyFilter = 0;
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

    v2u getSize() const
    {
        return v2u(width, height);
    }

    img::PixelFormat getFormat() const
	{
		return format;
	}

    TextureSettings getParameters() const
    {
        return texSettings;
    }

	bool hasMipMaps() const
	{
		return texSettings.hasMipMaps;
	}
	bool isRenderTarget() const
	{
		return texSettings.isRenderTarget;
	}

    virtual void bind() = 0;
    virtual void unbind() = 0;

	virtual void uploadData(img::Image *img, img::ImageModifier *imgMod = nullptr) = 0;
	virtual void uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod = nullptr) = 0;

    virtual std::vector<img::Image *> downloadData() = 0;
    virtual void regenerateMipMaps() = 0;

    virtual void updateParameters(const TextureSettings &newTexSettings, bool updateLodBias, bool updateAnisotropy) = 0;

    bool operator==(const Texture *other) const;
};

}
