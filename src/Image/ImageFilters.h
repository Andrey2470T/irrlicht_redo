#pragma once

#include "ImageModifier.h"

// The functions taken from src/client/imagefilters.h[.cpp]
namespace img
{

/* Fill in RGB values for transparent pixels, to correct for odd colors
 * appearing at borders when blending.  This is because many PNG optimizers
 * like to discard RGB values of transparent pixels, but when blending then
 * with non-transparent neighbors, their RGB values will show up nonetheless.
 *
 * This function modifies the original image in-place.
 *
 * Parameter "threshold" is the alpha level below which pixels are considered
 * transparent. Should be 127 when the texture is used with ALPHA_CHANNEL_REF,
 * 0 when alpha blending is used.
 */
void imageCleanTransparent(img::Image *src, u8 threshold, img::ImageModifier *mdf);

/* Returns the gamma-correct average color of the image, with transparent pixels
 * ignored. */
img::color8 imageAverageColor(const img::Image *img, img::ImageModifier *mdf);

/* Scale a region of an image into another image, using nearest-neighbor with
 * anti-aliasing; treat pixels as crisp rectangles, but blend them at boundaries
 * to prevent non-integer scaling ratio artifacts.  Note that this may cause
 * some blending at the edges where pixels don't line up perfectly, but this
 * filter is designed to produce the most accurate results for both upscaling
 * and downscaling.
 */
void imageScaleNNAA(img::Image *src, const recti &srcrect, img::Image *dest, ImageModifier *mdf);

/* Check and align image to npot2 if required by hardware
 * @param image image to check for npot2 alignment
 * @param driver driver to use for image operations
 * @return image or copy of image aligned to npot2
 */
img::Image *Align2Npot2(img::Image *image, ImageModifier *mdf);

img::Image *applyCleanScalePowerOf2(img::Image *src, const v2i &src_size,
    const v2i &dest_size, img::ImageModifier *mdf);

}
