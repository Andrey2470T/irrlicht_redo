#pragma once

#include "ITexture.h"
#include "Image/CImage.h"
#include "Image/CColorConverter.h"
#include "Common.h"

namespace video
{

class Texture2D : public ITexture
{
protected:
	IImage *imgCache;

    u8 msaa = 0;
public:
    Texture2D(video::VideoDriver *driver, const std::string &name,
		const core::dimension2du &size, ECOLOR_FORMAT format, u8 msaa_n=0);
    Texture2D(video::VideoDriver *driver, const std::string &name,
		IImage *image, const TextureSettings &settings=TextureSettings());

	~Texture2D()
	{
		if (imgCache)
			imgCache->drop();
	}

	E_TEXTURE_TYPE getType() const override
	{
		return ETT_2D;
	}

	void bind() override;

	void unbind() override;

	void uploadData(IImage *img, u8 mipLevel=0) override;
	void uploadSubData(u32 x, u32 y, IImage *img, u8 mipLevel=0) override;

	std::vector<IImage *> downloadData() override;
	void regenerateMipMaps() override;

	void updateParameters(
		const TextureSettings &newTexSettings,
		bool updateLodBias, bool updateAnisotropy) override;

	//void resize(u32 newWidth, u32 newHeight, img::ImageModifier *imgMod);
	//Texture2D *copy(const std::string &name="");

	u32 tex2D() const
	{
		return msaa > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}
private:
    void initTexture(IImage *image);
};

}
