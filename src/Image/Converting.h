#include <SDL_image.h>
#include "Image.h"
#include <map>

namespace img
{
	static std::map<u32, u32> formatsEnumsMap = {
		{SDL_PIXELFORMAT_RGB888, PF_RGB8},
		{SDL_PIXELFORMAT_RGBA8888, PF_RGBA8},
		{SDL_PIXELFORMAT_INDEX8, PF_INDEX_RGBA8}
	};

	Image *convertSDLSurfaceToImage(SDL_Surface *surf)
	{
		SDL_PixelFormat *sdl_format = surf->format;
		PixelFormat format = static_cast<PixelFormat>(formatsEnumsMap.at(sdl_format->format));
        u32 w = static_cast<u32>(surf->w);
        u32 h = static_cast<u32>(surf->h);
		u8 *data = static_cast<u8*>(surf->pixels);
		
		return new Image(format, w, h, data);
	}
	
	SDL_Surface *convertImageToSDLSurface(Image *img)
	{
		s32 pixelBits = static_cast<s32>(pixelFormatInfo.at(img->getFormat()).size);
        s32 w = static_cast<s32>(img->getWidth());
        s32 h = static_cast<s32>(img->getHeight());
		s32 pitch = static_cast<s32>(getDataSizeFromFormat(img->getFormat(), img->getWidth(), img->getHeight()));
		void *data = img->getData();
		u32 redMask = getRedMask(img->getFormat());
		u32 greenMask = getGreenMask(img->getFormat());
		u32 blueMask = getBlueMask(img->getFormat());
		u32 alphaMask = getAlphaMask(img->getFormat());
		
		
		return SDL_CreateRGBSurfaceFrom(data, w, h, pixelBits, pitch, redMask, greenMask, blueMask, alphaMask);
	}
}
