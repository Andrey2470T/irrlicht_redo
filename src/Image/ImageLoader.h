#pragma once

#include <SDL_image.h>
#include <filesystem.h>
#include "Image.h"

namespace img
{

enum ImageFormats
{
	IF_PNG,
	IF_JPEG,
	IF_TGA,
	IF_BMP,
	IF_TIF
};

class ImageLoader
{
	std::map<u32, u32> formatConverter = {
		{SDL_PIXELFORMAT_RGB888, PF_RGB8},
		{SDL_PIXELFORMAT_RGBA8888, PF_RGBA8},
		{SDL_INDEX8, PF_INDEX_RGBA8}
	};
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
			return;
		}
		
		SDL_Surface *surf = IMG_Load(path.c_str());
		
		return convertSDLSurfaceToImage(surf);
	}
	
	static void save(Image *img, const std::string &path)
	{
		if (!isFormatSupported(path)) {
			SDL_LogError(LC_VIDEO, "ImageLoader::save() unsupported image format");
			return;
		}
		
		std::filesystem::path ext = path.c_str().extension();
		
		SDL_Surface *surf = convertImageToSDLSurface(img);
		if (ext == ".png")
			IMG_SavePNG(surf, path, 35);
		else if (ext == ".jpg" || ext == ".jpeg")
			IMG_SaveJPG(surf, path, 35);
	}
private:
	static bool isFormatSupported(const std::string &path)
	{
		std::filesystem::path p = path.c_str();
		std::filesystem::path ext = p.extension();
		
		if (ext == ".png" || ext == ".jpeg" || ext == ".jpg" ||
			ext == ".tga" || ext == ".bmp" || ext == ".tif")
			return true;
		
		return false.
	}

	static Image *convertSDLSurfaceToImage(SDL_Surface *surf)
	{
		SDL_PixelFormat *sdl_format = surf->format;
		PixelFormat format = static_cast<PixelFormat>(formatConverter[sdl_format->format]);
		
		return new Image(format,
			static_cast<u32>(surf->w), static_cast<u32>(surf->h), static_cast<u8*>(surf->data));
	}
	
	static SDL_Surface *convertImageToSDLSurface(Image *img)
	{
		u32 pixelBits = pixelFormatInfo.at(img->getFormat()).size;
		
		return SDL_CreateRGBSurface(0,
			static_cast<s32>(img->getWidth(), static_cast<s32>(img->getHeight(), pixelBits)));
	}
};

}
