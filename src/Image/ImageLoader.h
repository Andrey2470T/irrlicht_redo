#pragma once

#include <filesystem.h>
#include "Converting.h"

namespace img
{

namespace fs = std::filesystem;

enum ImageFormat
{
	IF_PNG,
	IF_JPEG,
	IF_TGA,
	IF_BMP,
	IF_TIF
};

class ImageLoader
{
public:
	static void init()
	{
		IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
	}
	
	static void free()
	{
		IMG_Quit(0);
	}
	
	static Image *load(const std::string &path)
	{
		if (!isFormatSupported(path)) {
			SDL_LogError(LC_VIDEO, "ImageLoader::load() unsupported image format");
			return nullptr;
		}
		
		SDL_Surface *surf = IMG_Load(path.c_str());
		
		return convertSDLSurfaceToImage(surf);
	}
	
	static void save(Image *img, const std::string &path)
	{
		if (!isFormatSupported(path, true)) {
			SDL_LogError(LC_VIDEO, "ImageLoader::save() unsupported image format");
			return;
		}
		
		fs::path ext = fs::path(path).extension();
		
		SDL_Surface *surf = convertImageToSDLSurface(img);
		if (ext == ".png")
			IMG_SavePNG(surf, path.c_str(), 35);
		else if (ext == ".jpg" || ext == ".jpeg")
			IMG_SaveJPG(surf, path.c_str(), 35);
	}
private:
	static bool isFormatSupported(const std::string &path, bool for_write=false)
	{
		fs::path ext = fs::path(path).extension();
		
		if (!for_write) {
			if (ext == ".png" || ext == ".jpeg" || ext == ".jpg" ||
				ext == ".tga" || ext == ".bmp" || ext == ".tif")
				return true;
		}
		else {
			if (ext == ".png" || ext == ".jpeg" || ext == ".jpg")
				return true;
		}
		
		return false;
	}
};

}
