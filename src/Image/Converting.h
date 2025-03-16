#pragma once

#include <SDL_image.h>
#include "Image.h"

namespace img
{
	static std::map<u32, u32> formatsEnumsMap = {
		{SDL_PIXELFORMAT_RGB888, PF_RGB8},
		{SDL_PIXELFORMAT_RGBA8888, PF_RGBA8},
		{SDL_PIXELFORMAT_INDEX8, PF_INDEX_RGBA8}
	};

    // Note: converting doesn't copy the input data, but just transform it to another form
	Image *convertSDLSurfaceToImage(SDL_Surface *surf);

	SDL_Surface *convertImageToSDLSurface(Image *img);

	// Convert the numerical u32 color representation (ARGB) to the color8 object
	color8 colorU32NumberToObject(u32 color);

	u32 colorObjectToU32Number(color8 color);

	color8 colorfToColor8(colorf c);

	colorf color8ToColorf(color8 c);
}
