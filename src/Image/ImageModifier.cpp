#include "ImageModifier.h"
#include "ResizeFilters.h"

namespace img
{

// Returns a pixel from the image
color8 ImageModifier::getPixel(const Image *img, u32 x, u32 y) const
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height) {
		SDL_LogWarn(LC_VIDEO, "ImageModifier::getPixel() coordinates are out of image canvas");
		return color8(PF_RGB8);
	}

	u8 *data = img->getData();

	switch(img->getFormat()) {
		case PF_RGB8:
			u8 *pixel = data[y * 3 * width + 3 * x];
			return color8(PF_RGB8, *pixel, *pixel++, *pixel++);
		case PF_RGBA8:
			u8 *pixel = data[y * 4 * width + 4 * x];
			return color8(PF_RGBA8, *pixel, *pixel++, *pixel++, *pixel++);
		case PF_INDEX_RGBA8:
			return img->getPalette()->colors.at(*(data[y * width + x]));
		default:
			SDL_LogWarn(LC_VIDEO, "ImageModifier::getPixel() unsupported/unknown format");
			return color8(PF_RGB8);
	}
}

//! Sets a pixel for the image
void ImageModifier::setPixel(
	Image *img,
	u32 x, u32 y,
	const color8 &color)
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (x >= width || y >= height) {
		SDL_LogWarn(LC_VIDEO, "ImageModifier::setPixel() coordinates are out of image canvas");
		return;
	}

	PixelFormat format = img->getFormat();
	u8 *data = img->getData();

	u8 *pixel = nullptr;
	color8 newColor(format);

	if (format == PF_RGB8) {
		pixel = data[y * 3 * width + 3 * x];
		newColor = color8(format, *pixel, *(pixel)+1, *(pixel)+2, 0);
	}
	else if (format == PF_RGBA8) {
		pixel = data[y * 4 * width + 4 * x];
		newColor = color8(format, *pixel, *(pixel)+1, *(pixel)+2, *(pixel)+3);
	}
	else if (format == PF_INDEX_RGBA) {
		data[y * width + x] = color.R();
		return;
	}
	else {
		SDL_LogWarn(LC_VIDEO, "ImageModifier::setPixel() unsupported/unknown format");
		return;
	}

	newColor = doBlend<u8>(color, newColor, Mode);

	*pixel = newColor.R();
	*(pixel+1) = newColor.G();
	*(pixel+2) = newColor.B();
	*(pixel+3) = newColor.A();
}

// Fills the whole (or part) of the image with the given color
// Absence of the passed rect means filling the whole image
void ImageModifier::fill(
	Image *img,
	const color8 &color,
	const utils::rectu *rect)
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();

	if (!rect) {
		for (u32 x = 0; x < width; x++)
			for (u32 y = 0; y < height; y++)
				setPixel(img, x, y, color);
	}
	else {
		utils::rectu img_rect(utils::v2u(width, height));
		if (!img_rect.isRectInside(*rect)) {
			SDL_LogWarn(LC_VIDEO, "ImageModifier::fill() sub rect is outside of image canvas");
			return;
		}

		for (u32 x = rect->ULC.X; x < rect->LRC.X; x++)
			for (u32 y = rect->ULC.Y; y < rect->LRC.Y; y++)
				setPixel(img, x, y, color);
	}
}

// Scales (up or down) the image before the given rect.
// The convolution algorithm is used with one of filter types.
void ImageModifier::resize(Image *img, const utils::rectu &rect, RESAMPLE_FILTER filter)
{
	f32 scaleX, scaleY = 1.0f, 1.0f;
	f32 fscaleX, fscaleY = scaleX, scaleY;
	
	utils::v2u imgSize = img->getSize();

	if (imgSize.X != rect.getWidth()) {
		scaleX = imgSize.X / (1.0f)rect.getWidth();
		fscaleX = std::max(1.0f, scaleX);
	}
	if (imgSize.Y != rect.getHeight()) {
		scaleY = imgSize.Y / (1.0f)rect.getHeight();
		fscaleY = std::max(1.0f, scaleY);
	}

	if (scaleX == 1.0f || scaleY == 1.0f) {
		SDL_LogWarn(LC_VIDEO, "ImageModifier::resize() no need in scaling (scale factor = 1.0)");
		return;
	}
	
	f32 radius = getFilterRadius(filter);
	f32 radiusX = radius * fscaleX;
	f32 radiusY = radius * fscaleY;
	
	auto axisScale = [&] (Image *inImg, Image *outImg, u8 axis)
	{
		f32 scale, fscale;
		utils::v2u outImgSize = outImg->getSize();
		u32 inImgWidth = inImg->getWidth();
		
		if (axis == 0) {
			scale = scaleX;
			fscale = fscaleX;
		}
		else {
			scale = scaleY;
			fscale = fscaleY;
		}
		
		f32 ss = 1.0f / fscale;
		
		u32 axis1 = axis == 0 ? outImgSize.X : outImgSize.Y;
		u32 axis2 = axis == 0 ? outImgSize.Y : outImgSize.X;

		for (u32 imgOut_axis = 0; imgOut_axis < axis1; imgOut_axis++) {
			f32 imgIn_center = (imgOut_axis + 0.5f) * scale;
			
			u32 min = std::max(0.0f, (s32)std::floor(imgOut_center - radius));
			u32 max = std::min((s32)std::ceil(imgOut_center + radius), inImgWidth);
			
			std::vector<f32> weights = Kernel(imgOut_center, inImgWidth, ss, filter);
			
			f32 w_s = 0.0f;
			
			for (f32 w : weights)
				w_s += w;
			
			if (w_s == 0.0f)
				w_s = 1.0f;
			else
				w_s = 1.0f / w_s;
			
			for (u32 imgOut_axis2 = 0; imgOut_axis2 < axis2; imgOut_axis2++) {
				color8 resColor(img->getFormat());
				
				for (u32 r = min; r < max; r++) {
					u32 x = axis == 0 ? r : imgOut_axis2;
					u32 y = axis == 0 ? imgOut_axis2 : r;
					
					resColor += getPixel(inImg, x, y) * weights[r - min];
				}
				
				resColor.R((u32)std::floor(resColor.R() * w_s + 0.5f));
				resColor.G((u32)std::floor(resColor.G() * w_s + 0.5f));
				resColor.B((u32)std::floor(resColor.B() * w_s + 0.5f));
				
				x = axis == 0 ? imgOut_axis : imgOut_axis2;
				y = axis == 0 ? imgOut_axis2 : imgOut_axis;
				
				setPixel(outImg, x, y, resColor);
			}
		}
	};
	
	Image *newImg = nullptr;
	
	if (scaleX != 1.0f) {
		newImg = new Image(img->getFormat(), rect.getWidth(), img->getHeight());
		axisScale(img, newImg, 0);
	}
	if (scaleY != 1.0f) {
		Image *newImgCopy = nullptr;
		if (!newImg) {
			newImgCopy = new Image(img->getFormat(), img->getWidth(), rect.getHeight());
			axisScale(img, newImgCopy, 1);
		}
		else {
			newImgCopy = new Image(img->getFormat(), newImg->getWidth(), rect.getHeight());
			axisScale(newImg, newImgCopy, 1);
			delete newImg;
		}
		
		newImg = newImgCopy;
	}
	
	return newImg;
}
}
