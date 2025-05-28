#pragma once

#include "Texture.h"

namespace render
{

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

class TextureCubeMap : public Texture
{
protected:
	std::array<std::unique_ptr<img::Image>, CMF_COUNT> imgCache;
public:
	TextureCubeMap(const std::string &name, u32 width, u32 height, img::PixelFormat format);
	TextureCubeMap(const std::string &name, std::array<std::unique_ptr<img::Image>, CMF_COUNT> images,
        const TextureSettings &settings=TextureSettings());
	
	void bind() const override
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	}

	void unbind() const override
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void uploadData(img::Image *img, img::ImageModifier *imgMod = nullptr) override {}
	void uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod = nullptr) override {}

    std::vector<img::Image *> downloadData() override;
    void regenerateMipMaps() override {};

    void updateParameters(const TextureSettings &newTexSettings, bool updateLodBias, bool updateAnisotropy) override;
private:
	void initTexture(const std::array<img::Image *, CMF_COUNT> &data = std::array<img::Image *, CMF_COUNT>());
};

}
