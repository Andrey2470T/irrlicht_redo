#include "ImageLoader.h"
#include "Converting.h"

namespace img
{

std::vector<std::string> SIE = {
	".png", ".jpeg", ".jpg", ".tga", ".bmp", ".tif"
};

Image *ImageLoader::load(const std::string &path)
{
	if (!isFormatSupported(path)) {
		ErrorStream << "ImageLoader::load() unsupported image format\n";
		return nullptr;
	}

	SDL_Surface *surf = IMG_Load(path.c_str());

	return convertSDLSurfaceToImage(surf);
}

Image *ImageLoader::loadFromMem(void *mem, s32 size)
{
    SDL_RWops *rw = SDL_RWFromMem(mem, size);
    SDL_Surface *surf = IMG_Load_RW(rw, 0);

    return convertSDLSurfaceToImage(surf);
}

void ImageLoader::save(Image *img, const std::string &path)
{
	if (!isFormatSupported(path, true)) {
		ErrorStream << "ImageLoader::save() unsupported image format\n";
		return;
	}

	fs::path ext = fs::path(path).extension();

	SDL_Surface *surf = convertImageToSDLSurface(img);
	if (ext == SIE[IF_PNG])
        IMG_SavePNG(surf, path.c_str());
	else if (ext == SIE[IF_JPG] || ext == SIE[IF_JPEG])
		IMG_SaveJPG(surf, path.c_str(), 35);
}

bool ImageLoader::isFormatSupported(const std::string &path, bool for_write)
{
	fs::path ext = fs::path(path).extension();

	if (!for_write) {
		if (ext == SIE[IF_PNG] || ext == SIE[IF_JPEG] || ext == SIE[IF_JPG] ||
			ext == SIE[IF_TGA] || ext == SIE[IF_BMP] || ext == SIE[IF_TIF])
			return true;
	}
	else {
		if (ext == SIE[IF_PNG] || ext == SIE[IF_JPEG] || ext == SIE[IF_JPG])
			return true;
	}

	return false;
}

}
