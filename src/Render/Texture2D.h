#pragma once

#include "Texture.h"

namespace image
{
struct Palette;
}

namespace render
{

class Texture2D : public Texture
{
protected:
	std::unique_ptr<img::Image> imgCache;

    u8 msaa = 0;
public:
    Texture2D(const std::string &name, u32 width, u32 height, img::PixelFormat format, u8 msaa_n);
    Texture2D(const std::string &name, std::unique_ptr<img::Image> image, const TextureSettings &settings=TextureSettings());

	TextureType getType() const override
	{
		return TT_2D;
	}

	void bind() const override
	{
        glBindTexture(tex2D(), texID);
	}

	void unbind() const override
	{
        glBindTexture(tex2D(), 0);
	}

	void uploadData(img::Image *img, img::ImageModifier *imgMod = nullptr) override;
	void uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod = nullptr) override;

    std::vector<img::Image *> downloadData() override;
    void regenerateMipMaps() override;

    void updateParameters(const TextureSettings &newTexSettings, bool updateLodBias, bool updateAnisotropy) override;
    
    Texture2D *copy(const std::string &name="");

    u32 tex2D() const
    {
        return msaa > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    }
private:
    void initTexture(img::Image *image);
    void convertIndicesToColors(img::Palette *palette, u8 *data, v2u size);
};

}
