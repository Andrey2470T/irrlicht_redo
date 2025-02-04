#include <SDL_image.h>
#include "Image.h"

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

	// Convert the numerical u32 color representation (ARGB) to the color8 object
	color8 colorU32NumberToObject(u32 color)
	{
		u8 alpha = color >> 24;
		u8 red = (color >> 16) & 0xFF;
		u8 green = (color >> 8) & 0xFF;
		u8 blue = color & 0xFF;

		return color8(PF_RGBA8, red, green, blue, alpha);
	}

	u32 colorObjectToU32Number(color8 color)
	{
		u32 coloru32;

		coloru32 |= ((u32)color.A() << 24);
		coloru32 |= ((u32)color.R() << 16);
		coloru32 |= ((u32)color.G() << 8);
		coloru32 |= ((u32)color.B());

		return coloru32;
	}

	color8 colorfToColor8(colorf c)
	{
		u8 red = static_cast<u8>(round32(c.R() * 255.0f));
		u8 green = static_cast<u8>(round32(c.G() * 255.0f));
		u8 blue = static_cast<u8>(round32(c.B() * 255.0f));
		u8 alpha = static_cast<u8>(round32(c.A() * 255.0f));

		return color8(PF_RGBA8, red, green, blue, alpha);
	}

	colorf color8ToColorf(color8 c)
	{
		f32 red = std::clamp<f32>(c.R() / 255.0f, 0.0f, 1.0f);
		f32 green = std::clamp<f32>(c.G() / 255.0f, 0.0f, 1.0f);
		f32 blue = std::clamp<f32>(c.B() / 255.0f, 0.0f, 1.0f);
		f32 alpha = std::clamp<f32>(c.A() / 255.0f, 0.0f, 1.0f);
	}
}
