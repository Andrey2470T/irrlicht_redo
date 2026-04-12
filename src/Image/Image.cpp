// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "Image.h"
#include "IWriteFile.h"
#include "irrString.h"
#include "CColorConverter.h"
#include "CBlit.h"
#include "IFileSystem.h"
#include "Logger.h"
#include "SoftwareDriver2_helper.h"

#include "CImageLoaderJPG.h"
#include "CImageLoaderPNG.h"
#include "CImageLoaderTGA.h"
#include "CImageWriterJPG.h"
#include "CImageWriterPNG.h"

#include <vector>
#include <cassert>


namespace video
{

//! Constructor from raw data
Image::Image(ECOLOR_FORMAT format, const core::dimension2d<u32> &size, void *data,
		bool ownForeignMemory, bool deleteMemory) :
        Format(format), Size(size), DeleteMemory(deleteMemory)
{
    BytesPerPixel = pixelFormatsInfo[Format].size / 8;
    Pitch = BytesPerPixel * Size.Width;

	if (ownForeignMemory) {
		assert(data);
		Data = reinterpret_cast<u8*>(data);
		if (reinterpret_cast<uintptr_t>(data) % sizeof(u32) != 0)
            g_irrlogger->log("Image created with foreign memory that's not aligned", ELL_WARNING);
	} else {
		const u32 dataSize = getDataSizeFromFormat(Format, Size.Width, Size.Height);
		const u32 allocSize = align_next(dataSize, 16);

		// allocate as u32 to ensure enough alignment when casted
		Data = reinterpret_cast<u8 *>(new u32[allocSize / 4]);
		memcpy(Data, data, dataSize);
		DeleteMemory = true;
	}
}

//! Constructor of empty image
Image::Image(ECOLOR_FORMAT format, const core::dimension2d<u32> &size) :
        Format(format), Size(size), DeleteMemory(true)
{
    BytesPerPixel = pixelFormatsInfo[Format].size / 8;
    Pitch = BytesPerPixel * Size.Width;

	const u32 dataSize = getDataSizeFromFormat(Format, Size.Width, Size.Height);
	const u32 allocSize = align_next(dataSize, 16);

	Data = reinterpret_cast<u8 *>(new u32[allocSize / 4]);
}

Image::~Image()
{
    if (DeleteMemory)
        delete[] Data;
}

Image *Image::createFromFile(const io::path &filename, io::IFileSystem *fs)
{
    if (!filename.size())
        return nullptr;

    auto file = fs->createAndOpenFile(filename);
    if (!file) {
        g_irrlogger->log("Could not open file of image", filename, ELL_WARNING);
        return nullptr;
    }

    auto image = createImageFromFile(file, fs);
    file->drop();
    return image;
}

Image *Image::createFromMemory(const void *data, s32 len, const io::path &filename, io::IFileSystem *fs)
{
    auto file = fs->createMemoryReadFile(data, len, filename.c_str());
    if (!file) {
        g_irrlogger->log("Could not create memory file of image", filename, ELL_WARNING);
        return nullptr;
    }

    auto image = createImageFromFile(file, fs);
    file->drop();
    return image;
}

//! Writes the provided image to disk file
bool Image::writeImageToFile(Image *image, const io::path &filename, io::IFileSystem *fs, u32 param)
{
    auto file = fs->createAndWriteFile(filename);
    if (!file)
        return false;

    bool result = writeImageToFile(image, file, fs, param);
    file->drop();

    return result;
}

//! sets a pixel
void Image::setPixel(u32 x, u32 y, const SColor &color, bool blend)
{
	if (x >= Size.Width || y >= Size.Height)
		return;

	switch (Format) {
	case ECF_A1R5G5B5: {
		u16 *dest = (u16 *)(Data + (y * Pitch) + (x << 1));
		*dest = video::A8R8G8B8toA1R5G5B5(color.color);
	} break;

	case ECF_R5G6B5: {
		u16 *dest = (u16 *)(Data + (y * Pitch) + (x << 1));
		*dest = video::A8R8G8B8toR5G6B5(color.color);
	} break;

	case ECF_R8G8B8: {
		u8 *dest = Data + (y * Pitch) + (x * 3);
		dest[0] = (u8)color.getRed();
		dest[1] = (u8)color.getGreen();
		dest[2] = (u8)color.getBlue();
	} break;

	case ECF_A8R8G8B8: {
		u32 *dest = (u32 *)(Data + (y * Pitch) + (x << 2));
		*dest = blend ? PixelBlend32(*dest, color.color) : color.color;
	} break;

	case ECF_UNKNOWN:
        g_irrlogger->log("IImage::setPixel unknown format.", ELL_WARNING);
		return;

	default:
		break;
	}
}

//! returns a pixel
SColor Image::getPixel(u32 x, u32 y) const
{
	if (x >= Size.Width || y >= Size.Height)
		return SColor(0);

	switch (Format) {
	case ECF_A1R5G5B5:
		return A1R5G5B5toA8R8G8B8(((u16 *)Data)[y * Size.Width + x]);
	case ECF_R5G6B5:
		return R5G6B5toA8R8G8B8(((u16 *)Data)[y * Size.Width + x]);
	case ECF_A8R8G8B8:
		return ((u32 *)Data)[y * Size.Width + x];
	case ECF_R8G8B8: {
		// FIXME this interprets the memory as [R][G][B], whereas SColor is stored as
		// 0xAARRGGBB, meaning it is lies in memory as [B][G][R][A] on a little endian machine.
		u8 *p = Data + (y * 3) * Size.Width + (x * 3);
		return SColor(255, p[0], p[1], p[2]);
	}

	case ECF_UNKNOWN:
        g_irrlogger->log("IImage::getPixel unknown format.", ELL_WARNING);
		break;

	default:
		break;
	}

	return SColor(0);
}

//! copies this surface into another at given position
void Image::copyTo(Image *target, const core::position2d<s32> &pos)
{
	if (!Blit(BLITTER_TEXTURE, target, 0, &pos, this, 0, 0) && target && pos.X == 0 && pos.Y == 0 &&
			CColorConverter::canConvertFormat(Format, target->getColorFormat())) {
		// No fast blitting, but copyToScaling uses other color conversions and might work
		core::dimension2du dim(target->getDimension());
		copyToScaling(target->getData(), dim.Width, dim.Height, target->getColorFormat(), target->getPitch());
	}
}

//! copies this surface partially into another at given position
void Image::copyTo(Image *target, const core::position2d<s32> &pos, const core::rect<s32> &sourceRect, const core::rect<s32> *clipRect)
{
	Blit(BLITTER_TEXTURE, target, clipRect, &pos, this, &sourceRect, 0);
}

//! copies this surface into another, using the alpha mask, a cliprect and a color to add with
void Image::copyToWithAlpha(Image *target, const core::position2d<s32> &pos, const core::rect<s32> &sourceRect, const SColor &color, const core::rect<s32> *clipRect, bool combineAlpha)
{
	eBlitter op = combineAlpha ? BLITTER_TEXTURE_COMBINE_ALPHA : color.color == 0xFFFFFFFF ? BLITTER_TEXTURE_ALPHA_BLEND
																						   : BLITTER_TEXTURE_ALPHA_COLOR_BLEND;
	Blit(op, target, clipRect, &pos, this, &sourceRect, color.color);
}

//! copies this surface into another, if it has the exact same size and format.
bool Image::copyToNoScaling(void *target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch) const
{
	if (!target || !width || !height || !Size.Width || !Size.Height)
		return false;

	const u32 bpp = pixelFormatsInfo[format].size / 8;
	if (0 == pitch)
		pitch = width * bpp;

	if (!(Format == format && Size.Width == width && Size.Height == height))
		return false;

	if (pitch == Pitch) {
		memcpy(target, Data, (size_t)height * pitch);
	} else {
		u8 *tgtpos = (u8 *)target;
		u8 *srcpos = Data;
		const u32 bwidth = width * bpp;
		const u32 rest = pitch - bwidth;
		for (u32 y = 0; y < height; ++y) {
			// copy scanline
			memcpy(tgtpos, srcpos, bwidth);
			// clear pitch
			memset(tgtpos + bwidth, 0, rest);
			tgtpos += pitch;
			srcpos += Pitch;
		}
	}

	return true;
}

//! copies this surface into another, scaling it to the target image size
// note: this is very very slow.
void Image::copyToScaling(void *target, u32 width, u32 height, ECOLOR_FORMAT format, u32 pitch)
{
	if (!target || !width || !height || !Size.Width || !Size.Height)
		return;

	const u32 bpp = pixelFormatsInfo[format].size / 8;
	if (0 == pitch)
		pitch = width * bpp;

	if (copyToNoScaling(target, width, height, format, pitch))
		return;

	// NOTE: Scaling is coded to keep the border pixels intact.
	// Alternatively we could for example work with first pixel being taken at half step-size.
	// Then we have one more step here and it would be:
	//     sourceXStep = (f32)(Size.Width-1) / (f32)(width);
	//     And sx would start at 0.5f + sourceXStep / 2.f;
	//     Similar for y.
	// As scaling is done without any antialiasing it doesn't matter too much which outermost pixels we use and keeping
	// border pixels intact is probably mostly better (with AA the other solution would be more correct).
	// This is however unnecessary (and unexpected) for scaling to integer multiples, so don't do it there.
	f32 sourceXStep, sourceYStep;
	f32 sourceXStart = 0.f, sourceYStart = 0.f;
	if (width % Size.Width == 0)
		sourceXStep = (f32)(Size.Width) / (f32)(width);
	else {
		sourceXStep = width > 1 ? (f32)(Size.Width - 1) / (f32)(width - 1) : 0.f;
		sourceXStart = 0.5f; // for rounding to nearest pixel
	}
	if (height % Size.Height == 0)
		sourceYStep = (f32)(Size.Height) / (f32)(height);
	else {
		sourceYStep = height > 1 ? (f32)(Size.Height - 1) / (f32)(height - 1) : 0.f;
		sourceYStart = 0.5f; // for rounding to nearest pixel
	}

	s32 yval = 0, syval = 0;
	f32 sy = sourceYStart;
	for (u32 y = 0; y < height; ++y) {
		f32 sx = sourceXStart;
		for (u32 x = 0; x < width; ++x) {
			CColorConverter::convert_viaFormat(Data + syval + ((s32)sx) * BytesPerPixel, Format, 1, ((u8 *)target) + yval + (x * bpp), format);
			sx += sourceXStep;
		}
		sy += sourceYStep;
		syval = (s32)(sy)*Pitch;
		yval += pitch;
	}
}

//! copies this surface into another, scaling it to the target image size
// note: this is very very slow.
void Image::copyToScaling(Image *target)
{
	if (!target)
		return;

	const core::dimension2d<u32> &targetSize = target->getDimension();

	if (targetSize == Size) {
		copyTo(target);
		return;
	}

	copyToScaling(target->getData(), targetSize.Width, targetSize.Height, target->getColorFormat());
}

//! copies this surface into another, scaling it to fit it.
void Image::copyToScalingBoxFilter(Image *target, s32 bias, bool blend)
{
	const core::dimension2d<u32> destSize = target->getDimension();

	const f32 sourceXStep = (f32)Size.Width / (f32)destSize.Width;
	const f32 sourceYStep = (f32)Size.Height / (f32)destSize.Height;

	s32 fx = core::ceil32(sourceXStep);
	s32 fy = core::ceil32(sourceYStep);
	f32 sx;
	f32 sy;

	sy = 0.f;
	for (u32 y = 0; y != destSize.Height; ++y) {
		sx = 0.f;
		for (u32 x = 0; x != destSize.Width; ++x) {
			target->setPixel(x, y,
					getPixelBox(core::floor32(sx), core::floor32(sy), fx, fy, bias), blend);
			sx += sourceXStep;
		}
		sy += sourceYStep;
	}
}

//! fills the surface with given color
void Image::fill(const SColor &color)
{
	u32 c;

	switch (Format) {
	case ECF_A1R5G5B5:
		c = color.toA1R5G5B5();
		c |= c << 16;
		break;
	case ECF_R5G6B5:
		c = video::A8R8G8B8toR5G6B5(color.color);
		c |= c << 16;
		break;
	case ECF_A8R8G8B8:
		c = color.color;
		break;
	case ECF_R8G8B8: {
		u8 rgb[3];
		CColorConverter::convert_A8R8G8B8toR8G8B8(&color, 1, rgb);
		const u32 size = getImageDataSizeInBytes();
		for (u32 i = 0; i < size; i += 3) {
			memcpy(Data + i, rgb, 3);
		}
		return;
	} break;
	default:
		// TODO: Handle other formats
		return;
	}
	memset32(Data, c, getImageDataSizeInBytes());
}

void Image::flip(E_FLIP_AXIS axis)
{
    if (axis == EFA_Y) {
        // Vertical flip
        u8 *srcA = static_cast<u8 *>(Data);
        u8 *srcB = srcA + (Size.Height - 1) * Pitch;

        std::vector<u8> tmpBuffer(Pitch);
        for (u32 i = 0; i < Size.Height; i += 2) {
            memcpy(tmpBuffer.data(), srcA, Pitch);
            memcpy(srcA, srcB, Pitch);
            memcpy(srcB, tmpBuffer.data(), Pitch);
            srcA += Pitch;
            srcB -= Pitch;
        }
    }
    else {
        // Horizontal flip
        const u32 bytesPerPixel = BytesPerPixel;
        std::vector<u8> tmpBuffer(bytesPerPixel);

        for (u32 y = 0; y < Size.Height; ++y)
        {
            u8 *rowStart = Data + (y * Pitch);
            u8 *left = rowStart;
            u8 *right = rowStart + (Size.Width - 1) * bytesPerPixel;

            for (u32 x = 0; x < Size.Width / 2; ++x)
            {
                // Swap pixels
                memcpy(tmpBuffer.data(), left, bytesPerPixel);
                memcpy(left, right, bytesPerPixel);
                memcpy(right, tmpBuffer.data(), bytesPerPixel);

                left += bytesPerPixel;
                right -= bytesPerPixel;
            }
        }
    }
}

//! get a filtered pixel
inline SColor Image::getPixelBox(s32 x, s32 y, s32 fx, s32 fy, s32 bias) const
{
	SColor c;
	s32 a = 0, r = 0, g = 0, b = 0;

	for (s32 dx = 0; dx != fx; ++dx) {
		for (s32 dy = 0; dy != fy; ++dy) {
			c = getPixel(core::s32_min(x + dx, Size.Width - 1),
					core::s32_min(y + dy, Size.Height - 1));

			a += c.getAlpha();
			r += c.getRed();
			g += c.getGreen();
			b += c.getBlue();
		}
	}

	s32 sdiv = core::u32_log2(fx * fy);

	a = core::s32_clamp((a >> sdiv) + bias, 0, 255);
	r = core::s32_clamp((r >> sdiv) + bias, 0, 255);
	g = core::s32_clamp((g >> sdiv) + bias, 0, 255);
	b = core::s32_clamp((b >> sdiv) + bias, 0, 255);

	c.set(a, r, g, b);
	return c;
}

IImageLoader *selectLoader(io::IReadFile *file)
{
    std::vector<IImageLoader *> loaders;
    loaders.push_back(ImgJPGLoader.get());
    loaders.push_back(ImgPNGLoader.get());
    loaders.push_back(ImgTGALoader.get());

    for (auto &loader : loaders) {
        if (!loader->isALoadableFileExtension(file->getFileName()))
            continue;

        file->seek(0); // reset file position which might have changed due to previous loadImage calls
        // avoid warnings if extension is wrong
        if (!loader->isALoadableFileFormat(file))
            continue;

        file->seek(0);

        return loader;
    }

    return nullptr;
}

IImageWriter *selectWriter(io::IWriteFile *file)
{
    std::vector<IImageWriter *> writers;
    writers.push_back(ImgJPGWriter.get());
    writers.push_back(ImgPNGWriter.get());

    for (auto &writer : writers) {
        if (writer->isAWriteableFileExtension(file->getFileName()))
            return writer;
    }

    return nullptr;
}

Image *Image::createImageFromFile(io::IReadFile *file, io::IFileSystem *fs)
{
    if (!file)
        return nullptr;

    if (auto reader = selectLoader(file))
        return reader->loadImage(file);
    return nullptr;
}

//! Writes the provided image to a file.
bool Image::writeImageToFile(Image *image, io::IWriteFile *file, io::IFileSystem *fs, u32 param)
{
    if (!file)
        return false;

    bool written = false;
    if (auto writer = selectWriter(file))
        written = writer->writeImage(file, image, param);
    return written;
}

} // end namespace video
