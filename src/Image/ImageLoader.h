#pragma once

#include <SDL_image.h>
#include <filesystem.h>
#include "Image.h"

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
	static std::map<u32, u32> formatsEnumsMap = {
		{SDL_PIXELFORMAT_RGB888, PF_RGB8},
		{SDL_PIXELFORMAT_RGBA8888, PF_RGBA8},
		{SDL_PIXELFORMAT_INDEX8, PF_INDEX_RGBA8}
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

	static Image *convertSDLSurfaceToImage(SDL_Surface *surf)
	{
		SDL_PixelFormat *sdl_format = surf->format;
		PixelFormat format = static_cast<PixelFormat>(formatsEnumsMap.at(sdl_format->format));
		u32 w, h = static_cast<u32>(surf->w), static_cast<u32>(surf->h);
		u8 *data = static_cast<u8*>(surf->data);
		
		return new Image(format, w, h, data);
	}
	
	static SDL_Surface *convertImageToSDLSurface(Image *img)
	{
		s32 pixelBits = static_cast<s32>(pixelFormatInfo.at(img->getFormat()).size);
		s32 w, h = static_cast<s32>(img->getWidth()), static_cast<s32>(img->getHeight());
		s32 pitch = static_cast<s32>(getDataSizeFromFormat(img->getFormat(), img->getWidth(), img->getHeight()));
		void *data = img->getData();
		u32 redMask = getRedMask(img->getFormat());
		u32 greenMask = getGreenMask(img->getFormat());
		u32 blueMask = getBlueMask(img->getFormat());
		u32 alphaMask = getAlphaMask(img->getFormat());
		
		
		return SDL_CreateRGBSurfaceFrom(data, w, h, pixelBits, pitch, redMask, greenMask, blueMask, alphaMask);
	}
};

}
