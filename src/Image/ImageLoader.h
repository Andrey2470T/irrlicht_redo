#pragma once

#include "FilesystemVersions.h"
#include <SDL_image.h>
#include "Image.h"

namespace img
{

enum ImageFormat : u8
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
    static bool init()
	{
        if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) < 0) {
            ErrorStream << "ImageLoader::init() failed to init SDL2_image: " << SDL_GetError() << "\n";
            return false;
        }

        return true;
	}

	static void free()
	{
        IMG_Quit();
	}

	static Image *load(const std::string &path);
    static Image *loadFromMem(void *mem, s32 size);

	static void save(Image *img, const std::string &path);
private:
	static bool isFormatSupported(const std::string &path, bool for_write=false);
};

}
