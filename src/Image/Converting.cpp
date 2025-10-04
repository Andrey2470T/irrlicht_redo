#include "Converting.h"
#include "Image/ImageModifier.h"

namespace img
{
    std::string sdlFormatToString(u32 format)
	{
        std::string format_s;

        switch(format) {
        case((u32)SDL_PIXELFORMAT_INDEX1LSB):
            format_s = "SDL_PIXELFORMAT_INDEX1LSB";
            break;
        case((u32)SDL_PIXELFORMAT_INDEX1MSB):
            format_s = "SDL_PIXELFORMAT_INDEX1MSB";
            break;
        /*case((u32)SDL_PIXELFORMAT_INDEX2LSB):
            format_s = "SDL_PIXELFORMAT_INDEX2LSB";
            break;
        case((u32)SDL_PIXELFORMAT_INDEX2MSB):
            format_s = "SDL_PIXELFORMAT_INDEX2MSB";*/
            break;
        case((u32)SDL_PIXELFORMAT_INDEX4LSB):
            format_s = "SDL_PIXELFORMAT_INDEX4LSB";
            break;
        case((u32)SDL_PIXELFORMAT_INDEX4MSB):
            format_s = "SDL_PIXELFORMAT_INDEX4MSB";
            break;
        case((u32)SDL_PIXELFORMAT_INDEX8):
            format_s = "SDL_PIXELFORMAT_INDEX8";
            break;
        case((u32)SDL_PIXELFORMAT_RGB332):
            format_s = "SDL_PIXELFORMAT_RGB332";
            break;
        case((u32)SDL_PIXELFORMAT_RGB444):
            format_s = "SDL_PIXELFORMAT_RGB444";
            break;
        case((u32)SDL_PIXELFORMAT_XBGR4444):
            format_s = "SDL_PIXELFORMAT_XBGR4444";
            break;
        case((u32)SDL_PIXELFORMAT_RGB555):
            format_s = "SDL_PIXELFORMAT_RGB555";
            break;
        case((u32)SDL_PIXELFORMAT_BGR555):
            format_s = "SDL_PIXELFORMAT_BGR555";
            break;
        case((u32)SDL_PIXELFORMAT_ARGB4444):
            format_s = "SDL_PIXELFORMAT_ARGB4444";
            break;
        case((u32)SDL_PIXELFORMAT_RGBA4444):
            format_s = "SDL_PIXELFORMAT_RGBA4444";
            break;
        case((u32)SDL_PIXELFORMAT_ABGR4444):
            format_s = "SDL_PIXELFORMAT_ABGR4444";
            break;
        case((u32)SDL_PIXELFORMAT_BGRA4444):
            format_s = "SDL_PIXELFORMAT_BGRA4444";
            break;
        case((u32)SDL_PIXELFORMAT_ARGB1555):
            format_s = "SDL_PIXELFORMAT_ARGB1555";
            break;
        case((u32)SDL_PIXELFORMAT_RGBA5551):
            format_s = "SDL_PIXELFORMAT_RGBA5551";
            break;
        case((u32)SDL_PIXELFORMAT_ABGR1555):
            format_s = "SDL_PIXELFORMAT_ABGR1555";
            break;
        case((u32)SDL_PIXELFORMAT_BGRA5551):
            format_s = "SDL_PIXELFORMAT_BGRA5551";
            break;
        case((u32)SDL_PIXELFORMAT_RGB565):
            format_s = "SDL_PIXELFORMAT_RGB565";
            break;
        case((u32)SDL_PIXELFORMAT_BGR565):
            format_s = "SDL_PIXELFORMAT_BGR565";
            break;
        case((u32)SDL_PIXELFORMAT_RGB24):
            format_s = "SDL_PIXELFORMAT_RGB24";
            break;
        case((u32)SDL_PIXELFORMAT_BGR24):
            format_s = "SDL_PIXELFORMAT_BGR24";
            break;
        case((u32)SDL_PIXELFORMAT_RGB888):
            format_s = "SDL_PIXELFORMAT_RGB888";
            break;
        case((u32)SDL_PIXELFORMAT_RGBX8888):
            format_s = "SDL_PIXELFORMAT_RGBX8888";
            break;
        case((u32)SDL_PIXELFORMAT_BGR888):
            format_s = "SDL_PIXELFORMAT_BGR888";
            break;
        case((u32)SDL_PIXELFORMAT_BGRX8888):
            format_s = "SDL_PIXELFORMAT_BGRX8888";
            break;
        case((u32)SDL_PIXELFORMAT_ARGB8888):
            format_s = "SDL_PIXELFORMAT_ARGB8888";
            break;
        case((u32)SDL_PIXELFORMAT_RGBA8888):
            format_s = "SDL_PIXELFORMAT_RGBA8888";
            break;
        case((u32)SDL_PIXELFORMAT_ABGR8888):
            format_s = "SDL_PIXELFORMAT_ABGR8888";
            break;
        case((u32)SDL_PIXELFORMAT_BGRA8888):
            format_s = "SDL_PIXELFORMAT_BGRA8888";
            break;
        case((u32)SDL_PIXELFORMAT_ARGB2101010):
            format_s = "SDL_PIXELFORMAT_ARGB2101010";
            break;
        case((u32)SDL_PIXELFORMAT_YV12):
            format_s = "SDL_PIXELFORMAT_YV12";
            break;
        case((u32)SDL_PIXELFORMAT_IYUV):
            format_s = "SDL_PIXELFORMAT_IYUV";
            break;
        case((u32)SDL_PIXELFORMAT_YUY2):
            format_s = "SDL_PIXELFORMAT_YUY2";
            break;
        case((u32)SDL_PIXELFORMAT_UYVY):
            format_s = "SDL_PIXELFORMAT_UYVY";
            break;
        case((u32)SDL_PIXELFORMAT_YVYU):
            format_s = "SDL_PIXELFORMAT_YVYU";
            break;
        case((u32)SDL_PIXELFORMAT_NV12):
            format_s = "SDL_PIXELFORMAT_NV12";
            break;
        case((u32)SDL_PIXELFORMAT_NV21):
            format_s = "SDL_PIXELFORMAT_NV21";
            break;
        case((u32)SDL_PIXELFORMAT_EXTERNAL_OES):
            format_s = "SDL_PIXELFORMAT_EXTERNAL_OES";
            break;
        default:
            format_s = "SDL_PIXELFORMAT_UNKNOWN";
            break;
        }

        return format_s;
    }

    SDL_Surface* convertToTargetFormat(SDL_Surface* src_surface);

    // Note: converting doesn't copy the input data, but just transform it to another form
	Image *convertSDLSurfaceToImage(SDL_Surface *surf)
	{
		SDL_PixelFormat *sdl_format = surf->format;
		
        auto it = formatsEnumsMap.find(sdl_format->format);

        std::string formatStr = sdlFormatToString(sdl_format->format);
        InfoStream << "Loaded the image format: " << formatStr << "\n";
		if (it == formatsEnumsMap.end()) {
            ErrorStream << "convertSDLSurfaceToImage() unsupported pixel format:" << sdlFormatToString(sdl_format->format) << "\n";
			return nullptr;
		}

		PixelFormat format = static_cast<PixelFormat>(it->second);
        u32 w = static_cast<u32>(surf->w);
        u32 h = static_cast<u32>(surf->h);
        u8 *data = static_cast<u8*>(surf->pixels);

        // load the SDL palette
        Palette *palette = nullptr;
        if (sdl_format->format == SDL_PIXELFORMAT_INDEX8) {
            SDL_Palette *sdl_palette = sdl_format->palette;

            std::vector<img::color8> colors;

            InfoStream << "convertSDLSurfaceToImage 1 palette size: " << sdl_palette->ncolors << "\n";
            for (s32 i = 0; i < sdl_palette->ncolors; i++) {
                SDL_Color sdl_color = sdl_palette->colors[i];
                colors.emplace_back(img::PF_RGBA8, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
            }
            InfoStream << "convertSDLSurfaceToImage 2\n";

            palette = new Palette(true, colors.size(), colors);
            InfoStream << "convertSDLSurfaceToImage 3\n";
        }

        auto img = new Image(format, w, h, data, true, palette);

        auto localImgMod = new img::ImageModifier();
        auto flipped_y = localImgMod->flip(img, FD_Y);
        delete img;
        delete localImgMod;

        return flipped_y;
	}

	SDL_Surface *convertImageToSDLSurface(Image *img)
	{
		PixelFormat format = img->getFormat();
		
		auto it = std::find_if(formatsEnumsMap.begin(), formatsEnumsMap.end(), [&] (const std::pair<u32, u32> &p)
		{
			return p.second == format;
        });
		if (it == formatsEnumsMap.end()) {
			ErrorStream << "convertImageToSDLSurface() unsupported pixel format:" << format << "\n";
			return nullptr;
		}
		s32 pixelBits = static_cast<s32>(pixelFormatInfo.at(img->getFormat()).size);
        s32 w = static_cast<s32>(img->getWidth());
        s32 h = static_cast<s32>(img->getHeight());
		s32 pitch = static_cast<s32>(getDataSizeFromFormat(img->getFormat(), img->getWidth(), img->getHeight()));
        u8 *data = new u8[w * h * pixelBits / 8];
        memcpy(data, img->getData(), w * h * pixelBits / 8);
		u32 redMask = getRedMask(img->getFormat());
		u32 greenMask = getGreenMask(img->getFormat());
		u32 blueMask = getBlueMask(img->getFormat());
		u32 alphaMask = getAlphaMask(img->getFormat());

		return SDL_CreateRGBSurfaceFrom(data, w, h, pixelBits, pitch, redMask, greenMask, blueMask, alphaMask);
	}

    SDL_Surface* convertToRGBA8888(SDL_Surface* src_surface) {
        u32 sdlformat = src_surface->format->format;
        InfoStream << "convertToTargetFormat: 3.1\n";
        if (!src_surface ||
            (sdlformat != SDL_PIXELFORMAT_ABGR8888 &&
            sdlformat != SDL_PIXELFORMAT_BGRA8888 &&
            sdlformat != SDL_PIXELFORMAT_ARGB8888)) {
            return nullptr;
        }

        InfoStream << "convertToTargetFormat: 3.2\n";
        SDL_Surface* dst_surface = SDL_CreateRGBSurfaceWithFormat(
            0, src_surface->w, src_surface->h, 32, SDL_PIXELFORMAT_RGBA8888);

        if (!dst_surface) {
            return nullptr;
        }

        InfoStream << "convertToTargetFormat: 3.3\n";
        SDL_LockSurface(src_surface);
        SDL_LockSurface(dst_surface);

        u32* src_pixels = (u32*)src_surface->pixels;
        u32* dst_pixels = (u32*)dst_surface->pixels;
        int pixel_count = src_surface->w * src_surface->h;

        u32 r, g, b, a;

        for (int i = 0; i < pixel_count; i++) {
            u32 pixel = src_pixels[i];

            if (sdlformat == SDL_PIXELFORMAT_ABGR8888) {
                // ABGR -> RGBA
                a = (pixel >> 24) & 0xFF;
                b = (pixel >> 16) & 0xFF;
                g = (pixel >> 8) & 0xFF;
                r = pixel & 0xFF;
            }
            else if (sdlformat == SDL_PIXELFORMAT_BGRA8888)
            {
                // BGRA -> RGBA
                b = (pixel >> 24) & 0xFF;
                g = (pixel >> 16) & 0xFF;
                r = (pixel >> 8) & 0xFF;
                a = pixel & 0xFF;
            }
            else
            {
                // ARGB -> RGBA
                a = (pixel >> 24) & 0xFF;
                r = (pixel >> 16) & 0xFF;
                g = (pixel >> 8) & 0xFF;
                b = pixel & 0xFF;
            }
            dst_pixels[i] = (r << 24) | (g << 16) | (b << 8) | a;
        }

        SDL_UnlockSurface(dst_surface);
        SDL_UnlockSurface(src_surface);

        return dst_surface;
    }

    SDL_Surface* convertBGR888ToRGB888(SDL_Surface* src_surface) {
        if (!src_surface || src_surface->format->format != SDL_PIXELFORMAT_BGR888) {
            return NULL;
        }

        SDL_Surface* dst_surface = SDL_CreateRGBSurfaceWithFormat(
            0, src_surface->w, src_surface->h, 24, SDL_PIXELFORMAT_RGB888);

        if (!dst_surface) {
            return NULL;
        }

        SDL_LockSurface(src_surface);
        SDL_LockSurface(dst_surface);

        u8* src_pixels = (u8*)src_surface->pixels;
        u8* dst_pixels = (u8*)dst_surface->pixels;
        int pixel_count = src_surface->w * src_surface->h;

        for (int i = 0; i < pixel_count; i++) {
            // BGR -> RGB
            u8 b = src_pixels[i * 3];
            u8 g = src_pixels[i * 3 + 1];
            u8 r = src_pixels[i * 3 + 2];

            dst_pixels[i * 3] = r;
            dst_pixels[i * 3 + 1] = g;
            dst_pixels[i * 3 + 2] = b;
        }

        SDL_UnlockSurface(dst_surface);
        SDL_UnlockSurface(src_surface);

        return dst_surface;
    }

    SDL_Surface* convertToTargetFormat(SDL_Surface* src_surface) {
        InfoStream << "convertToTargetFormat: 1\n";
        if (!src_surface) return nullptr;
        InfoStream << "convertToTargetFormat: 2\n";

        u32 format = src_surface->format->format;

        /*switch (format) {
        case SDL_PIXELFORMAT_ABGR8888:
        case SDL_PIXELFORMAT_BGRA8888:
        case SDL_PIXELFORMAT_ARGB8888:
            InfoStream << "convertToTargetFormat: 3\n";
            return convertToRGBA8888(src_surface);
        case SDL_PIXELFORMAT_BGR888:
            return convertBGR888ToRGB888(src_surface);
            InfoStream << "convertToTargetFormat: 4\n";
        default:
            InfoStream << "convertToTargetFormat: 5\n";*/
            // Если формат уже целевой или неизвестный, возвращаем копию
            return SDL_ConvertSurfaceFormat(src_surface, format, 0);
        //}
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
		u32 coloru32 = 0;

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

        return colorf(red, green, blue, alpha);
	}
}
