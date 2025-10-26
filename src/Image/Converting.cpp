#include "Converting.h"
#include "Image/ImageModifier.h"
#include "Core/TimeCounter.h"

namespace img
{

    // Note: converting doesn't copy the input data, but just transform it to another form
    Image *convertSDLSurfaceToImage(SDL_Surface *surf, bool flipImage)
	{
		SDL_PixelFormat *sdl_format = surf->format;
		
        auto it = formatsEnumsMap.find(sdl_format->format);

        std::string formatStr = SDL_GetPixelFormatName(sdl_format->format);
        //InfoStream << "formatStr: " << formatStr << "\n";
        //InfoStream << "Loaded the image format: " << formatStr << "\n";
		if (it == formatsEnumsMap.end()) {
            ErrorStream << "convertSDLSurfaceToImage() unsupported pixel format:" << formatStr << "\n";
			return nullptr;
		}

        PixelFormat format = static_cast<PixelFormat>(it->second);
        u32 w = static_cast<u32>(surf->w);
        u32 h = static_cast<u32>(surf->h);
        u32 pitch = static_cast<u32>(surf->pitch);
        u8 *data = static_cast<u8*>(surf->pixels);
        surf->pixels = nullptr;
        //u32 rowSize = w * pixelSize;
        
        //InfoStream << "Surface info: SDL format=" << sdl_format->format << ", our format=" << format << ", w=" << w << ", h=" << h << ", pitch=" << pitch << ", pixelSize=" << pixelSize << ", BitsPerPixel=" << (int)sdl_format->BitsPerPixel << "\n";

        //core::InfoStream << "convertSDLSurfaceToImage: creating image, time: " << TimeCounter::getRealTime() << "\n";
        // load the SDL palette
        //core::InfoStream << "convertSDLSurfaceToImage 1, time: " << TimeCounter::getRealTime() << " \n";
        Palette *palette = nullptr;
        if (sdl_format->format == SDL_PIXELFORMAT_INDEX8) {
            SDL_Palette *sdl_palette = sdl_format->palette;

            std::vector<img::color8> colors;
            colors.resize(sdl_palette->ncolors);
            //core::InfoStream << "convertSDLSurfaceToImage: creating image 2, time: " << TimeCounter::getRealTime() << "\n";

            //InfoStream << "convertSDLSurfaceToImage 1 palette size: " << sdl_palette->ncolors << "\n";
            for (s32 i = 0; i < sdl_palette->ncolors; i++) {
                SDL_Color &sdl_color = sdl_palette->colors[i];
                colors[i].R(sdl_color.r);
                colors[i].G(sdl_color.g);
                colors[i].B(sdl_color.b);
                colors[i].A(sdl_color.a);
            }
            //InfoStream << "convertSDLSurfaceToImage 2\n";

            //core::InfoStream << "convertSDLSurfaceToImage: creating image 3, time: " << TimeCounter::getRealTime() << "\n";
            palette = new Palette(true, colors.size(), colors);
            //InfoStream << "convertSDLSurfaceToImage 3\n";
        }
        //core::InfoStream << "convertSDLSurfaceToImage 2, time: " << TimeCounter::getRealTime() << " \n";

        //InfoStream << "convertSDLSurfaceToImage:1\n";
        //core::InfoStream << "convertSDLSurfaceToImage: creating image 4, time: " << TimeCounter::getRealTime() << "\n";
        auto img = new Image(format, w, h, data, false, palette, pitch, sdl_format->format);
        //core::InfoStream << "convertSDLSurfaceToImage 3, time: " << TimeCounter::getRealTime() << " \n";
        //core::InfoStream << "convertSDLSurfaceToImage: created, flipping image, time: " << TimeCounter::getRealTime() << "\n";

        if (flipImage) {
            auto localImgMod = new img::ImageModifier();
            //InfoStream << "convertSDLSurfaceToImage:2\n";
            auto flipped_y = localImgMod->flip(img, FD_Y);
            //core::InfoStream << "convertSDLSurfaceToImage: created, flipped, deleting now, time: " << TimeCounter::getRealTime() << "\n";
            //InfoStream << "convertSDLSurfaceToImage:3\n";
            delete img;
            //InfoStream << "convertSDLSurfaceToImage:4\n";
            delete localImgMod;
            //InfoStream << "convertSDLSurfaceToImage:5\n";
            //core::InfoStream << "convertSDLSurfaceToImage: created, deleted, time: " << TimeCounter::getRealTime() << "\n";

            img = flipped_y;
        }
        //core::InfoStream << "convertSDLSurfaceToImage 4, time: " << TimeCounter::getRealTime() << " \n";

        return img;
	}

	SDL_Surface *convertImageToSDLSurface(Image *img)
	{
        u32 sdlFormat = img->getFormatsEnumsIndex();
		s32 pixelBits = static_cast<s32>(pixelFormatInfo.at(img->getFormat()).size);
        s32 w = static_cast<s32>(img->getWidth());
        s32 h = static_cast<s32>(img->getHeight());
        s32 pitch = static_cast<s32>(img->getPitch());

        return SDL_CreateRGBSurfaceWithFormatFrom(img->getData(), w, h, pixelBits, pitch, sdlFormat);
	}

    Image *convertIndexImageToRGBA(Image *img)
    {
        if (img->getFormat() != PF_INDEX_RGBA8)
            return nullptr;

        auto palette = img->getPalette();
        u32 width = img->getWidth();
        u32 height = img->getHeight();
        u32 pitch = img->getPitch();

        if (width * height == 0)
            return nullptr;

        PixelFormat format = palette->hasAlpha ? PF_RGBA8 : PF_RGB8;
        auto convImg = new Image(format, width, height);
        u8 size = pixelFormatInfo.at(format).size / 8;

        u8 *data = img->getData();
        u8 *convdata = convImg->getData();

        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u8 i = y * pitch + x;
                u8 i2 = y * width + x;
                auto found_color = palette->colors.at(data[i]);
                convdata[i2*size] = found_color.R();
                convdata[i2*size+1] = found_color.G();
                convdata[i2*size+2] = found_color.B();

                if (format == PF_RGBA8)
                    convdata[i2*size+3] = found_color.A();
            }
        }

        return convImg;
    }

    u8 *convertRGBAImageDataToIndex(Palette *palette, u8 *data, const v2u &size, u8 pitch)
    {
        u32 width = size.X;
        u32 height = size.Y;

        if (width * height == 0)
            return nullptr;

        u8 *convData = new u8[width * height];

        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u8 i = y * pitch + x;
                u8 i2 = y * width + x;

                u8 alpha = palette->hasAlpha ? data[i+3] : 0;
                img::color8 cur_color(img::PF_RGBA8, data[i], data[i+1], data[i+2], alpha);
                convData[i2] = palette->findColorIndex(cur_color);
            }
        }

        return convData;
    }

    color8 convertColorToIndexImageFormat(Image *img, color8 color)
    {
        auto img_format = img->getFormat();
        auto color_format = color.getFormat();

        if (img_format == color_format || img_format != PF_INDEX_RGBA8)
            return color;

        color8 target_c(PF_INDEX_RGBA8);
        target_c.R(img->getPalette()->findColorIndex(color));
        return target_c;
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
