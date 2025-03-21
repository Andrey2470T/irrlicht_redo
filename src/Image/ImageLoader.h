#pragma once

#include "FilesystemVersions.h"
#include <SDL_image.h>
#include "Image.h"

namespace img
{

enum ImageFormat
{
	IF_PNG,
	IF_JPEG,
	IF_JPG,
	IF_TGA,
	IF_BMP,
	IF_TIF
};

// Supported Image Extensions
extern std::vector<std::string> SIE;

class ImageLoader
{
public:
	static void init()
	{
		IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	}

	static void free()
	{
        IMG_Quit();
	}

	static Image *load(const std::string &path);

	static void save(Image *img, const std::string &path);
private:
	static bool isFormatSupported(const std::string &path, bool for_write=false);
};

}
