#include <algorithm>
#include "ImageModifier.h"

namespace img
{

// Returns a pixel from the image
color8 ImageModifier::getPixel(const Image *img, u32 x, u32 y) const
{
	u32 width = img->getWidth();
	u32 height = img->getHeight();
	PixelFormat format = img->getFormat();

	if (x >= width || y >= height) {
		ErrorStream << "ImageModifier::getPixel() coordinates are out of image canvas\n";
		return color8(format);
	}

    rectu clipRect(img->getClipPos(), img->getClipSize());
    if (!clipRect.isPointInside(v2u(x, y))) {
        ErrorStream << "ImageModifier::getPixel() coordinates are out of current clip region\n";
        return color8(format);
    }

	u8 *data = img->getData();

    switch(format) {
    case PF_R8:
        return color8(PF_R8, data[y * width + x]);
    case PF_RG8: {
        u8 *pixel = &data[y * 2 * width + 2 * x];
        return color8(PF_RG8, *pixel, *(pixel+1));
    }
    case PF_RGB8: {
         u8 *pixel = &data[y * 3 * width + 3 * x];
        return color8(PF_RGB8, *pixel, *(pixel+1), *(pixel+2));
    }
    case PF_RGBA8: {
         u8 *pixel = &data[y * 4 * width + 4 * x];
        return color8(PF_RGBA8, *pixel, *(pixel+1), *(pixel+2), *(pixel+3));
    }
    case PF_INDEX_RGBA8:
        return color8(PF_INDEX_RGBA8, data[y * width + x]);
    default:
        WarnStream << "ImageModifier::getPixel() unsupported/unknown format";
        return color8(format);
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
		WarnStream << "ImageModifier::setPixel() coordinates are out of image canvas\n";
		return;
	}

    rectu clipRect(img->getClipPos(), img->getClipSize());
    if (!clipRect.isPointInside(v2u(x, y))) {
        WarnStream << "ImageModifier::setPixel() coordinates are out of current clip region\n";
        return;
    }

    color8 newColor = getPixel(img, x, y);

    PixelFormat format = img->getFormat();

    if (format != PF_INDEX_RGBA8)
        newColor = doBlend(color, newColor, Mode);
    else {
        auto palette = img->getPalette();

        if (!palette) {
            newColor.R(color.R());
        }
        else {
            auto srcColor = palette->getColorByIndex(color.R());
            auto destColor = palette->getColorByIndex(newColor.R());
            newColor.R(palette->findColorIndex(doBlend(srcColor, destColor, Mode)));
        }
    }

	u8 *data = img->getData();
    u8 *pixel = nullptr;

    switch(format) {
    case PF_R8:
    case PF_INDEX_RGBA8: {
        data[y * width + x] = newColor.R();
        break;
    }
    case PF_RG8: {
        pixel = &data[y * 2 * width + 2 * x];
        *pixel = newColor.R();
        *(pixel+1) = newColor.G();
        break;
    }
    case PF_RGB8: {
        pixel = &data[y * 3 * width + 3 * x];
        *pixel = newColor.R();
        *(pixel+1) = newColor.G();
        *(pixel+2) = newColor.B();
        break;
    }
    case PF_RGBA8: {
        pixel = &data[y * 4 * width + 4 * x];
        *pixel = newColor.R();
        *(pixel+1) = newColor.G();
        *(pixel+2) = newColor.B();
        *(pixel+3) = newColor.A();
        break;
    default:
        WarnStream << "ImageModifier::setPixel() unsupported/unknown format\n";
        break;
    }}
}

// Return (only) a pixel color from the image
// If the format is indexed, it will extract the color from the palette, not its index
color8 ImageModifier::getPixelColor(const Image *img, u32 x, u32 y) const
{
	color8 c = getPixel(img, x, y);

	if (img->getFormat() == img::PF_INDEX_RGBA8)
		return img->getPalette()->colors.at(c.R());

	return c;
}

// Sets (only) a pixel color for the image
// If the format is indexed, it will find the closest pixel color from the palette, so "color" shoudln't be indexed
void ImageModifier::setPixelColor(Image *img, u32 x, u32 y, const color8 &color)
{
	if (img->getFormat() == img::PF_INDEX_RGBA8)
		setPixel(img, x, y, color8(img->getFormat(), img->getPalette()->findColorIndex(color)));
	else
		setPixel(img, x, y, color);
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
			WarnStream << "ImageModifier::fill() sub rect is outside of image canvas\n";
			return;
		}

		for (u32 x = rect->ULC.X; x < rect->LRC.X; x++)
			for (u32 y = rect->ULC.Y; y < rect->LRC.Y; y++)
				setPixel(img, x, y, color);
	}
}

// Copies the whole (or part) of the source image to the whole (or part) of the dest image
/// @param "srcImg" - image whose the pixel data is copied
/// @param "dstImg" - image where the copy happens to
/// @param "srcRect" - part of the image which will be copied
/// @param "dstRect" - part of the image where the copy will be done to
/// @param "allowScale" - if true, the copy will occur with scaling before the "dstRect" bounds (upscaling or downscaling)
bool ImageModifier::copyTo(
    Image *srcImg,
	Image *dstImg,
	const utils::rectu *srcRect,
	const utils::rectu *dstRect,
	bool allowScale)
{
    utils::rectu targetRect;

    if (srcRect) {
        targetRect.ULC = srcRect->ULC;
        targetRect.LRC = srcRect->LRC;
    }
    else {
        targetRect.ULC = utils::v2u(0, 0);
        targetRect.LRC = srcImg->getSize();
    }

    Image *srcPart = new Image(srcImg->getFormat(), targetRect.getWidth(), targetRect.getHeight(),
        img::black, srcImg->getPalette());

    for (u32 x = targetRect.ULC.X; x < targetRect.LRC.X; x++) {
        for (u32 y = targetRect.ULC.Y; y < targetRect.LRC.Y; y++)
            setPixel(srcPart, x - targetRect.ULC.X, y - targetRect.ULC.Y, getPixel(srcImg, x, y));
    }

    if (dstRect && targetRect.getSize() != dstRect->getSize()) {
		if (!allowScale) {
			ErrorStream << "ImageModifier::copyTo() copying to destination image with another size is not allowed\n";
            delete srcPart;
			return false;
		}

        resize(&srcPart, *dstRect, RF_BICUBIC);
	}

	for (u32 x = 0; x < srcPart->getWidth(); x++)
		for (u32 y = 0; y < srcPart->getHeight(); y++) {
			color8 curColor = getPixel(srcPart, x, y);

			if (!dstRect)
				setPixel(dstImg, x, y, curColor);
			else
				setPixel(dstImg, x + dstRect->ULC.X, y + dstRect->ULC.Y, curColor);
		}

    delete srcPart;

	return true;
}

// Compute next-higher power of 2 efficiently, e.g. for power-of-2 texture sizes.
// Public Domain: https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
inline u32 npot2(u32 orig) {
    orig--;
    orig |= orig >> 1;
    orig |= orig >> 2;
    orig |= orig >> 4;
    orig |= orig >> 8;
    orig |= orig >> 16;
    return orig + 1;
}

// Copies the whole (or part) of the image to the new image with the power-of-two dimensions
Image *ImageModifier::copyWith2NPot2Scaling(Image *img, const rectu *rect)
{
    v2u size;

    if (rect)
        size = rect->getSize();
    else
        size = img->getSize();

    u32 newWidth = npot2(size.X);
    u32 newHeight = npot2(size.Y);
    rectu scaledRect(0, 0, newWidth, newHeight);
    Image *scaledImg = new Image(img->getFormat(), newWidth, newHeight, img::black, img->getPalette());
    copyTo(img, scaledImg, rect, &scaledRect, true);

    return scaledImg;
}

// For more colorspace transformations, see for example
// <https://github.com/tobspr/GLSL-Color-Spaces/blob/master/ColorSpaces.inc.glsl>

inline f32 linear_to_srgb_component(f32 v)
{
    if (v > 0.0031308f)
        return 1.055f * powf(v, 1.0f / 2.4f) - 0.055f;
    return 12.92f * v;
}
inline f32 srgb_to_linear_component(f32 v)
{
    if (v > 0.04045f)
        return powf((v + 0.055f) / 1.055f, 2.4f);
    return v / 12.92f;
}

struct LUT8 {
    std::array<f32, 256> t;
    LUT8() {
        for (size_t i = 0; i < t.size(); i++)
            t[i] = srgb_to_linear_component(i / 255.0f);
    }
} srgb_to_linear_lut;

v3f srgb_to_linear(const color8 &col_srgb)
{
    return v3f(srgb_to_linear_lut.t[col_srgb.R()],
               srgb_to_linear_lut.t[col_srgb.G()],
               srgb_to_linear_lut.t[col_srgb.B()]);
}

color8 linear_to_srgb(const v3f col_linear)
{
    v3f col;
    // we can't LUT this without losing precision, but thankfully we call
    // it just once :)
    col.X = std::clamp(linear_to_srgb_component(col_linear.X) * 255.0f, 0.0f, 255.0f);
    col.Y = std::clamp(linear_to_srgb_component(col_linear.Y) * 255.0f, 0.0f, 255.0f);
    col.Z = std::clamp(linear_to_srgb_component(col_linear.Z) * 255.0f, 0.0f, 255.0f);

    return color8(PF_RGBA8, (u8)std::round(col.X), (u8)std::round(col.Y),
                       (u8)std::round(col.Z), 0xFF);
}

// Returns the gamma-correct average color of the image, with transparent pixels ignored
color8 ImageModifier::imageAverageColor(Image *img)
{
    auto size = img->getSize();

    u32 total = 0;
    v3f col_acc;
    // limit runtime cost
    const u32 stepx = std::max(1U, size.X / 16),
        stepy = std::max(1U, size.Y / 16);
    for (u32 x = 0; x < size.X; x += stepx) {
        for (u32 y = 0; y < size.Y; y += stepy) {
            color8 c = getPixelColor(img, x, y);
            if (c.A() > 0) {
                total++;
                col_acc += srgb_to_linear(c);
            }
        }
    }

    color8 ret;
    if (total > 0) {
        col_acc /= total;
        ret = linear_to_srgb(col_acc);
    }
    ret.A(255);
    return ret;
}

// Scales (up or down) the image before the given rect.
// The convolution algorithm is used with one of filter types.
void ImageModifier::resize(Image **img, const utils::rectu &rect, RESAMPLE_FILTER filter)
{
    /*f32 scaleX = 1.0f;
    f32 scaleY = 1.0f;
    f32 fscaleX = scaleX;
    f32 fscaleY = scaleY;

	utils::v2u imgSize = img->getSize();

	if (imgSize.X != rect.getWidth()) {
        scaleX = imgSize.X / (f32)rect.getWidth();
		fscaleX = std::max(1.0f, scaleX);
	}
	if (imgSize.Y != rect.getHeight()) {
        scaleY = imgSize.Y / (f32)rect.getHeight();
		fscaleY = std::max(1.0f, scaleY);
	}

	if (scaleX == 1.0f || scaleY == 1.0f) {
		WarnStream << "ImageModifier::resize() no need in scaling (scale factor = 1.0)\n";
        return;
    }*/

    f32 scaleX = (f32)rect.getWidth() / (*img)->getWidth();
    f32 scaleY = (f32)rect.getHeight() / (*img)->getHeight();

    if (scaleX == 1.0f && scaleY == 1.0f) {
        WarnStream << "ImageModifier::resize() no need in scaling (scale factor = 1.0)\n";
        return;
    }

	f32 radius = getFilterRadius(filter);

	auto axisScale = [&] (Image *inImg, Image *outImg, u8 axis)
	{
        f32 scale = (axis == 0) ? scaleX : scaleY;
        f32 fscale = std::max(1.0f, 1.0f / scale); // filter scale

        u32 outSize1 = (axis == 0) ? outImg->getWidth() : outImg->getHeight();
        u32 outSize2 = (axis == 0) ? outImg->getHeight() : outImg->getWidth();
        u32 inSize = (axis == 0) ? inImg->getWidth() : inImg->getHeight();

        f32 ss = 1.0f / fscale;

        for (u32 outIdx1 = 0; outIdx1 < outSize1; outIdx1++) {
            f32 inCenter = (outIdx1 + 0.5f) * scale;

            u32 min = std::max<s32>(0, (s32)std::floor(inCenter - radius));
            u32 max = std::min<s32>((s32)std::ceil(inCenter + radius), inSize);

            if (min >= max) continue;

            std::vector<f32> weights(max - min);
            f32 w_sum = 0.0f;

            // Вычисляем веса
            for (u32 i = 0; i < weights.size(); i++) {
                u32 x = min + i;
                f32 dist = (x - inCenter + 0.5f) * ss;

                switch (filter) {
                case RF_NEAREST: weights[i] = NearestFilter(dist); break;
                case RF_BILINEAR: weights[i] = BilinearFilter(dist); break;
                case RF_BICUBIC: weights[i] = BicubicFilter(dist); break;
                default: weights[i] = 0.0f; break;
                }
                w_sum += weights[i];
            }

            // Нормализация
            if (w_sum != 0.0f) {
                for (auto& w : weights) w /= w_sum;
            }
            // Применяем фильтр ко всем пикселям вдоль второй оси
            for (u32 outIdx2 = 0; outIdx2 < outSize2; outIdx2++) {
                color8 resColor((*img)->getFormat()); // Используем float для накопления

                for (u32 i = 0; i < weights.size(); i++) {
                    u32 r = min + i;
                    u32 x = (axis == 0) ? r : outIdx2;
                    u32 y = (axis == 0) ? outIdx2 : r;

                    InfoStream << "resize 1\n";
                    color8 curPixel = getPixelColor(inImg, x, y);
                    resColor += curPixel * weights[i];
                }

                u32 resultX = (axis == 0) ? outIdx1 : outIdx2;
                u32 resultY = (axis == 0) ? outIdx2 : outIdx1;

                 InfoStream << "resize 2\n";
                setPixelColor(outImg, resultX, resultY, resColor);
            }
        }
        /*f32 scale, fscale;
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

            u32 min = std::max<f32>(0.0f, (s32)std::floor(imgIn_center - radius));
            u32 max = std::min<f32>((s32)std::ceil(imgIn_center + radius), inImgWidth);

            std::vector<f32> weights = Kernel(imgIn_center, inImgWidth, ss, filter);

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

					color8 curPixel = getPixelColor(inImg, x, y);
					resColor += curPixel * weights[r - min];
				}

				resColor.R((u32)std::floor(resColor.R() * w_s + 0.5f));
				resColor.G((u32)std::floor(resColor.G() * w_s + 0.5f));
				resColor.B((u32)std::floor(resColor.B() * w_s + 0.5f));

                u32 x = axis == 0 ? imgOut_axis : imgOut_axis2;
                u32 y = axis == 0 ? imgOut_axis2 : imgOut_axis;

				setPixelColor(outImg, x, y, resColor);
			}
        }*/
	};

    Image *newImgScaledX = *img;
    bool wasScaledX = false;

	if (scaleX != 1.0f) {
        newImgScaledX = new Image((*img)->getFormat(), rect.getWidth(), (*img)->getHeight(),
            img::black, (*img)->getPalette());
        axisScale(*img, newImgScaledX, 0);
        wasScaledX = true;
	}

    Image *newImgScaledY = newImgScaledX;
	if (scaleY != 1.0f) {
        newImgScaledY = new Image((*img)->getFormat(), (*img)->getWidth(), rect.getHeight(),
            img::black, (*img)->getPalette());
        axisScale(newImgScaledX, newImgScaledY, 1);

        if (wasScaledX)
            delete newImgScaledX;
	}

    delete *img;
    *img = newImgScaledY;
}

// Rotates the given image by the angle multiple by 90 degrees
Image *ImageModifier::rotate(Image *img, ROTATE_ANGLE angle)
{
    u32 oldWidth = img->getWidth();
    u32 oldHeight = img->getHeight();
    u32 newWidth = oldWidth;
    u32 newHeight = oldHeight;

	if (angle == RA_90 || angle == RA_270)
		std::swap(newWidth, newHeight);

    Image *newImg = new Image(img->getFormat(), newWidth, newHeight, img::black, img->getPalette());

    utils::v2u oldCenter = img->getSize() / 2;
    utils::v2u newCenter = utils::v2u(newWidth, newHeight) / 2;

    u16 degrees = 0;

	if (angle == RA_90)
        degrees = 270;
	else if (angle == RA_180)
		degrees = 180;
	else if (angle == RA_270)
        degrees = 90;

    for (u32 x = 0; x < newWidth; x++) {
        for (u32 y = 0; y < newHeight; y++) {
            utils::v2u relPos = utils::v2u(x, y) - newCenter;
            relPos.rotateBy(degrees);
            utils::v2u oldPos = relPos + oldCenter;

            if (oldPos.X < oldWidth && oldPos.Y < oldHeight) {
                color8 curColor = getPixel(img, oldPos.X, oldPos.Y);
                setPixel(newImg, x, y, curColor);
            }
        }
    }

	return newImg;
}

Image *ImageModifier::flip(Image *img, FLIP_DIR dir)
{
    u32 width = img->getWidth();
    u32 height = img->getHeight();
    Image *newImg = new Image(img->getFormat(), width, height, img::black, img->getPalette());

	for (u32 x = 0; x < width; x++)
		for (u32 y = 0; y < height; y++) {
			color8 curColor = getPixel(img, x, y);
            utils::v2u reflectPoint = dir == FD_X ? utils::v2u(width - 1 - x, y) : utils::v2u(x, height - 1 - y);

            setPixel(newImg, reflectPoint.X, reflectPoint.Y, curColor);
		}

    return newImg;
}

Image *ImageModifier::crop(Image *img, const utils::rectu &rect)
{
	if (rect.getSize() == img->getSize())
		return img;

    Image *newImg = new Image(img->getFormat(), rect.getWidth(), rect.getHeight(), img::black, img->getPalette());
    copyTo(img, newImg, &rect);

	return newImg;
}

Image *ImageModifier::combine(Image *img1, Image *img2)
{
	if (img1->getFormat() != img2->getFormat()) {
		ErrorStream << "ImageModifier::combine() pixel formats of both images must be same\n";
        return nullptr;
	}
    Image *newImg = new Image(img2->getFormat(), img2->getWidth(), img2->getHeight(), img::black, img2->getPalette());
	copyTo(img2, newImg);
	copyTo(img1, img2, nullptr, nullptr, true);

	return newImg;
}
}
