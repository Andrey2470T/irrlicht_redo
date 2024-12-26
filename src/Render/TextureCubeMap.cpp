#include "TextureCubeMap.h"

namespace render
{

TextureCubeMap::TextureCubeMap(const std::string &name, u32 width, u32 height, img::PixelFormat format)
	: Texture(name, width, height, format)
{
	initTexture();
}

TextureCubeMap(const std::string &name, std::array<std::unique_ptr<img::Image>, CMF_COUNT> images,
		const TextureSettings &settings)
	: Texture(name, images.at(0)->getWidth(), images.at(0)->getHeight(), image.at(0)->getFormat(), settings)
{
	std::array<img::Image *, CMF_COUNT> imgs;
	for (u8 i = 0; i < CMF_COUNT; i++) {
		imgCache[i] = images.at(i).release();
		imgs[i] = imgCache[i].get();
	}

	initTexture(imgs);
}
