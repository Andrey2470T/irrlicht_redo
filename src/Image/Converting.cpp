#include "Converting.h"
#include "Image/ImageModifier.h"

namespace img
{

    void RGB8ToRGBA(u8 *srcData, u8 *dstData, u32 srci, u32 dsti, Palette *palette)
    {
        dstData[dsti] = srcData[srci];
        dstData[dsti+1] = srcData[srci+1];
        dstData[dsti+2] = srcData[srci+2];
        dstData[dsti+3] = 255;
    }

    void IndexToRGBA(u8 *srcData, u8 *dstData, u32 srci, u32 dsti, Palette *palette)
    {
        auto found_color = palette->getColorByIndex(srcData[srci]);
        dstData[dsti] = found_color.R();
        dstData[dsti+1] = found_color.G();
        dstData[dsti+2] = found_color.B();

        if (found_color.R() == 0 && found_color.G() == 0 && found_color.B() == 0 && found_color.A() == 255)
            dstData[dsti+3] = 0;
        else
            dstData[dsti+3] = found_color.A();
    }

    void RGB565ToRGBA(u8 *srcData, u8 *dstData, u32 srci, u32 dsti, Palette *palette)
    {
        u8 r = srcData[srci] >> 3;
        u8 g = (srcData[srci] & 0x7) << 3;
        g |= ((srcData[srci+1] & 0xE0) >> 5);
        u8 b = srcData[srci+1] & 0x1F;

        dstData[dsti] = r;
        dstData[dsti+1] = g;
        dstData[dsti+2] = b;
        dstData[dsti+3] = 255;
    }

    template<typename Func>
    u8 *convertImageDataToRGBA(u8 *data, u32 width, u32 height, u32 srcPitch,
        PixelFormat srcFormat, Func&& pixelFunc, Palette *palette=nullptr)
    {
        if (width * height == 0)
            return nullptr;

        u8 srcPixelSize = pixelFormatInfo[srcFormat].size / 8;
        u8 dstPixelSize = pixelFormatInfo[img::PF_RGBA8].size / 8;

        u32 dstPitch = dstPixelSize * width;
        u8 *convData = new u8[height * dstPitch];

        for (u32 y = 0; y < height; y++) {
            for (u32 x = 0; x < width; x++) {
                u32 srci = y * srcPitch + x * srcPixelSize;
                u32 dsti = y * dstPitch + x * dstPixelSize;

                pixelFunc(data, convData, srci, dsti, palette);
            }
        }

        return convData;
    }

    // Note: converting doesn't copy the input data, but just transform it to another form
    Image *convertSDLSurfaceToImage(SDL_Surface *surf, bool flipImage)
	{
		SDL_PixelFormat *sdl_format = surf->format;
		
        auto it = formatsEnumsMap.find(sdl_format->format);

        std::string formatStr = SDL_GetPixelFormatName(sdl_format->format);

		if (it == formatsEnumsMap.end()) {
            ErrorStream << "convertSDLSurfaceToImage() unsupported pixel format:" << formatStr << "\n";
			return nullptr;
		}

        PixelFormat format = static_cast<PixelFormat>(it->second);
        u32 w = static_cast<u32>(surf->w);
        u32 h = static_cast<u32>(surf->h);
        u32 pitch = static_cast<u32>(surf->pitch);
        u8 *data = (u8*)surf->pixels;

        Palette *palette = nullptr;
        if (sdl_format->format == SDL_PIXELFORMAT_INDEX8) {
            SDL_Palette *sdl_palette = sdl_format->palette;

            std::vector<color8> colors;
            colors.resize(sdl_palette->ncolors);

            for (s32 i = 0; i < sdl_palette->ncolors; i++) {
                SDL_Color &sdl_color = sdl_palette->colors[i];
                colors[i].R(sdl_color.r);
                colors[i].G(sdl_color.g);
                colors[i].B(sdl_color.b);
                colors[i].A(sdl_color.a);
            }

            palette = new Palette(true, colors.size(), colors);
        }

        u8 *convData = data;

        bool converted = false;
        if (sdl_format->format == SDL_PIXELFORMAT_RGB565) {
            convData = convertImageDataToRGBA(data, w, h, pitch, PF_RG8, RGB565ToRGBA);
            converted = true;
        }
        else if (format == PF_RGB8) {
            convData = convertImageDataToRGBA(data, w, h, pitch, PF_RGB8, RGB8ToRGBA);
            converted = true;
        }
        else if (format == PF_INDEX_RGBA8) {
            convData = convertImageDataToRGBA(data, w, h, pitch, PF_INDEX_RGBA8, IndexToRGBA, palette);
            converted = true;
        }

        if (converted)
            pitch = pixelFormatInfo[PF_RGBA8].size / 8 * w;

        if (sdl_format->format == SDL_PIXELFORMAT_INDEX8)
            delete palette;

        auto img = new Image(PF_RGBA8, w, h, convData, converted ? false : true,
            nullptr, pitch, SDL_PIXELFORMAT_ABGR8888);

        if (flipImage) {
            auto flipped_y = g_imgmod->flip(img, FD_Y);

            delete img;
            img = flipped_y;
        }

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

    Image *convertImageToRGBA(Image *img, bool &converted)
    {
        if (img->getFormat() == PF_RGBA8) {
            converted = false;
            return img;
        }

        u8 *convData = nullptr;

        u32 w = img->getWidth();
        u32 h = img->getHeight();
        switch(img->getFormat()) {
        case PF_RGB8:
            convData = convertImageDataToRGBA(img->getData(), w, h,
                img->getPitch(), PF_RGB8, RGB8ToRGBA);
            break;
        case PF_INDEX_RGBA8:
            convData = convertImageDataToRGBA(img->getData(), w, h,
                img->getPitch(), PF_INDEX_RGBA8, IndexToRGBA, img->getPalette());
            break;
        default:
            break;
        }

        if (!convData) {
            converted = false;
            return img;
        }

        u32 pitch = pixelFormatInfo[PF_RGBA8].size / 8 * w;
        converted = true;

        return new Image(PF_RGBA8, w, h, convData, false, nullptr, pitch, SDL_PIXELFORMAT_RGBA8888);
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
    color8 colorU32NumberToObject(u32 color, bool toRGBA)
	{
		u8 alpha = color >> 24;
		u8 red = (color >> 16) & 0xFF;
		u8 green = (color >> 8) & 0xFF;
		u8 blue = color & 0xFF;

        if (toRGBA)
            return color8(PF_RGBA8, alpha, green, blue, red);
        else
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
