#include <algorithm>
#include "Image/Converting.h"
#include "ImageModifier.h"

namespace img
{

void ImageModifier::blendWithPalette(const Image *img, const color8 &srcColor, color8 &dstColor)
{
    if (img->getFormat() != PF_INDEX_RGBA8)
        dstColor = doBlend(srcColor, dstColor, Mode);
    else {
        auto palette = img->getPalette();

        if (!palette) {
            dstColor.R(srcColor.R());
        }
        else {
            auto srcPaletteColor = palette->getColorByIndex(srcColor.R());
            auto dstPaletteColor = palette->getColorByIndex(dstColor.R());

            dstColor.R(palette->findColorIndex(doBlend(srcPaletteColor, dstPaletteColor, Mode)));
        }
    }
}

void ImageModifier::getPixelDirect(const Image *img, u32 x, u32 y, color8 &color) const
{
	auto data = img->getData();
	u32 pitch = img->getPitch();
	
	switch(img->getFormat()) {
    case PF_R8:
    case PF_INDEX_RGBA8: {
        color.R(data[y * pitch + x]);
        break;
    }
    case PF_RG8: {
        u8 *pixel = &data[y * pitch + 2 * x];
        color.R(*pixel);
        color.G(*(pixel+1));
        break;
    }
    case PF_RGB8: {
        u8 *pixel = &data[y * pitch + 3 * x];
        color.R(*pixel);
        color.G(*(pixel+1));
        color.B(*(pixel+2));
        break;
    }
    case PF_RGBA8: {
        u8 *pixel = &data[y * pitch + 4 * x];
        color.R(*pixel);
        color.G(*(pixel+1));
        color.B(*(pixel+2));
        color.A(*(pixel+3));
        break;
    }
    default:
        break;
    }
}

void ImageModifier::setPixelDirect(Image *img, u32 x, u32 y, color8 &dstColor)
{
	auto data = img->getData();
	u32 pitch = img->getPitch();
	
	switch(img->getFormat()) {
    case PF_R8:
    case PF_INDEX_RGBA8: {
        data[y * pitch + x] = dstColor.R();
        break;
    }
    case PF_RG8: {
        u8 *pixel = &data[y * pitch + 2 * x];
        *pixel = dstColor.R();
        *(pixel+1) = dstColor.G();
        break;
    }
    case PF_RGB8: {
        u8 *pixel = &data[y * pitch + 3 * x];
        *pixel = dstColor.R();
        *(pixel+1) = dstColor.G();
        *(pixel+2) = dstColor.B();
        break;
    }
    case PF_RGBA8: {
        u8 *pixel = &data[y * pitch + 4 * x];
        *pixel = dstColor.R();
        *(pixel+1) = dstColor.G();
        *(pixel+2) = dstColor.B();
        *(pixel+3) = dstColor.A();
        break;
    }
    default:
    	break;
    }
}

void ImageModifier::setPixelDirectWithBlend(Image *img, u32 x, u32 y, const color8 &srcColor, color8 &dstColor)
{
    blendWithPalette(img, srcColor, dstColor);
    setPixelDirect(img, x, y, dstColor);
}

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

    color8 color(format);
    getPixelDirect(img, x, y, color);

    return color;
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

    color8 newColor = convertColorToIndexImageFormat(img, color);

    getPixelDirect(img, x, y, newColor);
    setPixelDirectWithBlend(img, x, y, color, newColor);
}

// Return (only) a pixel color from the image
// If the format is indexed, it will extract the color from the palette, not its index
color8 ImageModifier::getPixelColor(const Image *img, u32 x, u32 y) const
{
	color8 c = getPixel(img, x, y);

	if (img->getFormat() == img::PF_INDEX_RGBA8)
        return img->getPalette()->getColorByIndex(c.R());

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
    const rectu *rect)
{
    color8 convColor = convertColorToIndexImageFormat(img, color);

    processPixelsBulk(img, rect,
        [img, &convColor, this] (u32 x, u32 y, color8 &curColor)
        {
            setPixelDirectWithBlend(img, x, y, convColor, curColor);
        });
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
    const rectu *srcRect,
    const rectu *dstRect,
	bool allowScale)
{
    if (srcImg->getFormat() != dstImg->getFormat()) {
        ErrorStream << "ImageModifier::copyTo() source and destination formats must match\n";
        return false;
    }

    rectu srcProcessRect = srcRect ? *srcRect : rectu(0, 0, srcImg->getWidth(), srcImg->getHeight());
    rectu dstProcessRect = dstRect ? *dstRect : rectu(0, 0, dstImg->getWidth(), dstImg->getHeight());

    if (srcProcessRect.getWidth() == 0 || srcProcessRect.getHeight() == 0 ||
        dstProcessRect.getWidth() == 0 || dstProcessRect.getHeight() == 0) {
        ErrorStream << "ImageModifier::copyTo() invalid source or destination region\n";
        return false;
    }

    srcProcessRect.clipAgainst(rectu(0, 0, srcImg->getWidth(), srcImg->getHeight()));
    dstProcessRect.clipAgainst(rectu(0, 0, dstImg->getWidth(), dstImg->getHeight()));

    bool needsScaling = allowScale && srcProcessRect.getSize() != dstProcessRect.getSize();
    Image *tempSrcImg = nullptr;

    if (needsScaling) {
        tempSrcImg = new Image(srcImg->getFormat(),
            srcProcessRect.getWidth(), srcProcessRect.getHeight(),
            img::black, srcImg->getPalette(), nullptr,
            srcImg->getFormatsEnumsIndex());

        processPixelsBulk(srcImg, &srcProcessRect,
            [tempSrcImg, &srcProcessRect, this](u32 x, u32 y, color8 &curColor) {
                u32 destX = x - srcProcessRect.ULC.X;
                u32 destY = y - srcProcessRect.ULC.Y;
                setPixelDirect(tempSrcImg, destX, destY, curColor);
            });

        v2u targetSize = dstProcessRect.getSize();
        resize(&tempSrcImg, targetSize, RF_BICUBIC);

        processPixelsBulk(tempSrcImg, nullptr,
            [dstImg, &dstProcessRect, this](u32 x, u32 y, color8 &curColor) {
                u32 destX = dstProcessRect.ULC.X + x;
                u32 destY = dstProcessRect.ULC.Y + y;

                if (destX < dstImg->getWidth() && destY < dstImg->getHeight()) {
                    color8 dstColor = getPixel(dstImg, destX, destY);
                    setPixelDirectWithBlend(dstImg, destX, destY, curColor, dstColor);
                }
            });
    } else {
        v2u actualCopySize(
            std::min(srcProcessRect.getSize().X, dstProcessRect.getSize().X),
            std::min(srcProcessRect.getSize().Y, dstProcessRect.getSize().Y)
        );

        processPixelsBulk(srcImg, &srcProcessRect,
            [dstImg, &srcProcessRect, &dstProcessRect, &actualCopySize, this](
                u32 srcX, u32 srcY, color8 &curColor) {

                    u32 relX = srcX - srcProcessRect.ULC.X;
                    u32 relY = srcY - srcProcessRect.ULC.Y;

                    if (relX >= actualCopySize.X || relY >= actualCopySize.Y)
                        return;

                    u32 destX = dstProcessRect.ULC.X + relX;
                    u32 destY = dstProcessRect.ULC.Y + relY;

                    if (destX < dstImg->getWidth() && destY < dstImg->getHeight()) {
                        color8 dstColor = getPixel(dstImg, destX, destY);
                        setPixelDirectWithBlend(dstImg, destX, destY, curColor, dstColor);
                    }
            });
    }

    if (tempSrcImg)
        delete tempSrcImg;

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
    Image *scaledImg = new Image(img->getFormat(), newWidth, newHeight, img::black, img->getPalette(),
        nullptr, img->getFormatsEnumsIndex());
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
void ImageModifier::resize(Image **img, const v2u &size, RESAMPLE_FILTER filter)
{
    f32 scaleX = (*img)->getWidth() / (f32)size.X;
    f32 scaleY = (*img)->getHeight() / (f32)size.Y;

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
            f32 inCenter = (outIdx1 + 0.5f) * scale - 0.5f;

            u32 min = std::max<s32>(0, (s32)std::floor(inCenter - radius));
            u32 max = std::min<s32>((s32)std::ceil(inCenter + radius), inSize - 1);

            if (min > max) {
                u32 nearestX = std::min((u32)std::round(inCenter), inSize - 1);
                for (u32 outIdx2 = 0; outIdx2 < outSize2; outIdx2++) {
                    u32 x = (axis == 0) ? nearestX : outIdx2;
                    u32 y = (axis == 0) ? outIdx2 : nearestX;
                    color8 pixel = getPixelColor(inImg, x, y);

                    u32 resultX = (axis == 0) ? outIdx1 : outIdx2;
                    u32 resultY = (axis == 0) ? outIdx2 : outIdx1;
                    setPixelColor(outImg, resultX, resultY, pixel);
                }
                continue;
            }

            std::vector<f32> weights(max - min + 1);
            f32 w_sum = 0.0f;

            for (u32 i = 0; i < weights.size(); i++) {
                u32 x = min + i;
                f32 dist = (x - inCenter) * ss;

                switch (filter) {
                case RF_NEAREST: weights[i] = NearestFilter(dist); break;
                case RF_BILINEAR: weights[i] = BilinearFilter(dist); break;
                case RF_BICUBIC: weights[i] = BicubicFilter(dist); break;
                default: weights[i] = 0.0f; break;
                }
                w_sum += weights[i];
            }

            if (w_sum == 0.0f) {
                // Uniform distribution
                f32 uniformWeight = 1.0f / weights.size();
                for (auto& w : weights) w = uniformWeight;
                w_sum = 1.0f;
            } else {
                for (auto& w : weights) w /= w_sum;
            }

            for (u32 outIdx2 = 0; outIdx2 < outSize2; outIdx2++) {
                color8 resColor((*img)->getFormat());

                for (u32 i = 0; i < weights.size(); i++) {
                    u32 r = min + i;
                    u32 x, y;
                    if (axis == 0) {
                        x = std::min<u32>(std::max<u32>(r, 0), (s32)inImg->getWidth() - 1);
                        y = std::min(outIdx2, inImg->getHeight() - 1);
                    } else {
                        x = std::min(outIdx2, inImg->getWidth() - 1);
                        y = std::min<u32>(std::max<u32>(r, 0), (s32)inImg->getHeight() - 1);
                    }

                    color8 curPixel = getPixelColor(inImg, x, y);
                    curPixel.R(curPixel.R() * weights[i]);
                    curPixel.G(curPixel.G() * weights[i]);
                    curPixel.B(curPixel.B() * weights[i]);
                    resColor += curPixel;
                }

                u32 resultX = (axis == 0) ? outIdx1 : outIdx2;
                u32 resultY = (axis == 0) ? outIdx2 : outIdx1;

                setPixelColor(outImg, resultX, resultY, resColor);
            }
        }
	};

    Image *newImgScaledX = *img;
    bool wasScaled = false;

    if (scaleX != 1.0f) {
        newImgScaledX = new Image((*img)->getFormat(), size.X, (*img)->getHeight(),
            img::black, (*img)->getPalette(), nullptr, (*img)->getFormatsEnumsIndex());
        axisScale(*img, newImgScaledX, 0);
        wasScaled = true;
    }

    Image *newImgScaledY = newImgScaledX;
    if (scaleY != 1.0f) {
        newImgScaledY = new Image(newImgScaledX->getFormat(), newImgScaledX->getWidth(), size.Y,
            img::black, newImgScaledX->getPalette(), nullptr, newImgScaledX->getFormatsEnumsIndex());
        axisScale(newImgScaledX, newImgScaledY, 1);

        if (wasScaled)
            delete newImgScaledX;
        //wasScaled = true;
    }

    //if (wasScaled)
    //    delete *img;

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

    Image *newImg = new Image(
        img->getFormat(), newWidth, newHeight, img::black, img->getPalette(), nullptr, img->getFormatsEnumsIndex());

    v2u oldCenter = img->getSize() / 2;
    v2u newCenter = v2u(newWidth, newHeight) / 2;

    u16 degrees = 0;

	if (angle == RA_90)
        degrees = 270;
	else if (angle == RA_180)
		degrees = 180;
	else if (angle == RA_270)
        degrees = 90;

    rectu area(0, 0, newWidth, newHeight);
    color8 oldColor(img->getFormat());
    processPixelsBulk(newImg, &area,
        [&] (u32 x, u32 y, color8 &curColor)
        {
            v2u relPos = v2u(x, y) - newCenter;
            relPos.rotateBy(degrees);
            v2u oldPos = relPos + oldCenter;

            if (oldPos.X < oldWidth && oldPos.Y < oldHeight) {
                getPixelDirect(img, oldPos.X, oldPos.Y, oldColor);
                setPixelDirect(newImg, x, y, oldColor);
            }
        });

	return newImg;
}

Image *ImageModifier::flip(Image *img, FLIP_DIR dir)
{
    u32 width = img->getWidth();
    u32 height = img->getHeight();
    Image *newImg = new Image(
        img->getFormat(), width, height, img::black, img->getPalette(), nullptr, img->getFormatsEnumsIndex());

    rectu area(0, 0, width, height);
    v2u reflectPoint;

    if (dir == FD_Y) {
        auto srcData = img->getData();
        auto dstData = newImg->getData();
        u32 srcPitch = img->getPitch();
        u32 dstPitch = newImg->getPitch();
        
        // Calculate actual bytes per row based on width and pixel format
        u32 pixelSize = pixelFormatInfo.at(img->getFormat()).size / 8;
        u32 rowBytes = width * pixelSize;

        for (u32 y = 0; y < height; y++) {
            auto srcRow = srcData + y * srcPitch;
            auto dstRow = dstData + (height - 1 - y) * dstPitch;

            memcpy(dstRow, srcRow, rowBytes);
        }
    }
    else {
        processPixelsBulk(img, &area,
            [&] (u32 x, u32 y, color8 &curColor)
            {
                reflectPoint.X = width - 1 - x;
                reflectPoint.Y = y;

                setPixelDirect(newImg, reflectPoint.X, reflectPoint.Y, curColor);
            });
    }

    return newImg;
}

Image *ImageModifier::crop(Image *img, const rectu &rect)
{
	if (rect.getSize() == img->getSize())
		return img;

    Image *newImg = new Image(
        img->getFormat(), rect.getWidth(), rect.getHeight(), img::black, img->getPalette(),
        nullptr, img->getFormatsEnumsIndex());
    copyTo(img, newImg, &rect);

	return newImg;
}

Image *ImageModifier::combine(Image *img1, Image *img2)
{
	if (img1->getFormat() != img2->getFormat()) {
		ErrorStream << "ImageModifier::combine() pixel formats of both images must be same\n";
        return nullptr;
	}
    Image *newImg = new Image(
        img2->getFormat(), img2->getWidth(), img2->getHeight(), img::black, img2->getPalette(),
        nullptr, img2->getFormatsEnumsIndex());
	copyTo(img2, newImg);
	copyTo(img1, img2, nullptr, nullptr, true);

	return newImg;
}

ImageModifier *g_imgmod = new ImageModifier();

}
