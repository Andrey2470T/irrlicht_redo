#include "ImageFilters.h"
#include "Utils/Bitmap.h"

namespace img
{

void imageCleanTransparent(img::Image *src, u8 threshold, img::ImageModifier *mdf)
{
    v2u size = src->getSize();
    Bitmap bitmap(size.X, size.Y);

    for (u32 x = 0; x < size.X; x++)
        for (u32 y = 0; y < size.Y; y++)
            if (mdf->getPixelColor(src, x, y).A() > threshold)
                bitmap.set(x, y, true);
    
    auto allOpaque = [&bitmap] ()
    {
        bool all = true;

        for (u32 i = 0; i < bitmap.size(); i++)
            if (bitmap.getByte(i) != 0xff) {
                all = false;
                break;
            }

        if (all)
            return; // all pixels are opaque
    };

    allOpaque();

    // Cap iterations to keep runtime reasonable, for higher-res textures we can
	// get away with filling less pixels.
	u32 iter_max = 11 - std::max(size.X, size.Y) / 16;
    iter_max = std::max(iter_max, 2u);

	// Then repeatedly look for transparent pixels, filling them in until
	// we're finished.
	for (u32 iter = 0; iter < iter_max; iter++) {

	    for (u32 y = 0; y < size.Y; y++)
	    for (u32 x = 0; x < size.X; x++) {
		    // Skip pixels we have already processed
		    if (bitmap.get(x, y))
			    continue;

		    // Sample size and total weighted r, g, b values
		    u32 ss = 0, sr = 0, sg = 0, sb = 0;

		    // Walk each neighbor pixel (clipped to image bounds)
		    for (u32 sy = (y < 1) ? 0 : (y - 1);
				    sy <= (y + 1) && sy < size.Y; sy++)
		    for (u32 sx = (x < 1) ? 0 : (x - 1);
				    sx <= (x + 1) && sx < size.X; sx++) {
			    // Ignore pixels we haven't processed
			    if (!bitmap.get(sx, sy))
				    continue;

			    // Add RGB values weighted by alpha IF the pixel is opaque, otherwise
			    // use full weight since we want to propagate colors.
			    img::color8 c = mdf->getPixelColor(src, sx, sy);
			    u32 a = c.A() <= threshold ? 255 : c.A();
			    ss += a;
			    sr += a * c.R();
                sg += a * c.G();
                sb += a * c.B();
		    }

		    // Set pixel to average weighted by alpha
		    if (ss > 0) {
			    img::color8 c = mdf->getPixelColor(src, x, y);
			    c.R(sr / ss);
			    c.G(sg / ss);
			    c.B(sb / ss);
			    mdf->setPixelColor(src, x, y, c);
			    bitmap.set(x, y, true);
		    }
	    }

	    allOpaque();
    }
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

v3f srgb_to_linear(const img::color8 &col_srgb)
{
    return v3f(srgb_to_linear_lut.t[col_srgb.R()],
        srgb_to_linear_lut.t[col_srgb.G()],
        srgb_to_linear_lut.t[col_srgb.B()]);
}

img::color8 linear_to_srgb(const v3f col_linear)
{
    v3f col;
    // we can't LUT this without losing precision, but thankfully we call
    // it just once :)
    col.X = std::clamp(linear_to_srgb_component(col_linear.X) * 255.0f, 0.0f, 255.0f);
    col.Y = std::clamp(linear_to_srgb_component(col_linear.Y) * 255.0f, 0.0f, 255.0f);
    col.Z = std::clamp(linear_to_srgb_component(col_linear.Z) * 255.0f, 0.0f, 255.0f);

    return img::color8(img::PF_RGBA8, (u8)std::round(col.X), (u8)std::round(col.Y),
        (u8)std::round(col.Z), 0xFF);
}

img::color8 imageAverageColor(const img::Image *img, img::ImageModifier *mdf)
{
    auto size = img->getSize();

    u32 total = 0;
    v3f col_acc;
    // limit runtime cost
    const u32 stepx = std::max(1U, size.X / 16),
        stepy = std::max(1U, size.Y / 16);
    for (u32 x = 0; x < size.X; x += stepx) {
        for (u32 y = 0; y < size.Y; y += stepy) {
            img::color8 c = mdf->getPixelColor(img, x, y);
            if (c.A() > 0) {
                total++;
                col_acc += srgb_to_linear(c);
            }
        }
    }

    img::color8 ret;
    if (total > 0) {
        col_acc /= total;
        ret = linear_to_srgb(col_acc);
    }
    ret.A(255);
    return ret;
}

void imageScaleNNAA(img::Image *src, const recti &srcrect, img::Image *dest, img::ImageModifier *mdf)
{
    f64 sx, sy, minsx, maxsx, minsy, maxsy, area, ra, ga, ba, aa, pw, ph, pa;
    u32 dy, dx;
    img::color8 pxl;

    // Cache rectangle boundaries.
    f64 sox = srcrect.ULC.X * 1.0;
    f64 soy = srcrect.ULC.Y * 1.0;
    f64 sw = srcrect.getWidth() * 1.0;
    f64 sh = srcrect.getHeight() * 1.0;

    // Walk each destination image pixel.
    // Note: loop y around x for better cache locality.
    auto size = dest->getSize();
    for (dy = 0; dy < size.Y; dy++)
    for (dx = 0; dx < size.X; dx++) {

        // Calculate floating-point source rectangle bounds.
        // Do some basic clipping, and for mirrored/flipped rects,
        // make sure min/max are in the right order.
        minsx = sox + (dx * sw / size.X);
        minsx = std::clamp(minsx, 0.0, sox + sw);
        maxsx = minsx + sw / size.X;
        maxsx = std::clamp(maxsx, 0.0, sox + sw);

        if (minsx > maxsx)
            std::swap(minsx, maxsx);
        minsy = soy + (dy * sh / size.Y);
        minsy = std::clamp(minsy, 0.0, soy + sh);
        maxsy = minsy + sh / size.Y;
        maxsy = std::clamp(maxsy, 0.0, soy + sh);

        if (minsy > maxsy)
            std::swap(minsy, maxsy);

        // Total area, and integral of r, g, b values over that area,
        // initialized to zero, to be summed up in next loops.
        area = 0;
        ra = 0;
        ga = 0;
        ba = 0;
        aa = 0;

        // Loop over the integral pixel positions described by those bounds.
        for (sy = floor(minsy); sy < maxsy; sy++)
        for (sx = floor(minsx); sx < maxsx; sx++) {

            // Calculate width, height, then area of dest pixel
            // that's covered by this source pixel.
            pw = 1;
            if (minsx > sx)
                pw += sx - minsx;
            if (maxsx < (sx + 1))
                pw += maxsx - sx - 1;
            ph = 1;
            if (minsy > sy)
                ph += sy - minsy;
            if (maxsy < (sy + 1))
                ph += maxsy - sy - 1;
            pa = pw * ph;

            // Get source pixel and add it to totals, weighted
            // by covered area and alpha.
            pxl = mdf->getPixelColor(src, (u32)sx, (u32)sy);
            area += pa;
            ra += pa * pxl.R();
            ga += pa * pxl.G();
            ba += pa * pxl.B();
            aa += pa * pxl.A();
        }

        // Set the destination image pixel to the average color.
        if (area > 0) {
            pxl.R(ra / area + 0.5);
            pxl.G(ga / area + 0.5);
            pxl.B(ba / area + 0.5);
            pxl.A(aa / area + 0.5);
        } else {
            pxl.R(0);
            pxl.G(0);
            pxl.B(0);
            pxl.A(0);
        }
        mdf->setPixelColor(dest, dx, dy, pxl);
    }
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

/* Check and align image to npot2 if required by hardware
 * @param image image to check for npot2 alignment
 * @param driver driver to use for image operations
 * @return image or copy of image aligned to npot2
 */
img::Image *Align2Npot2(img::Image *image, img::ImageModifier *mdf)
{
    if (!image)
        return image;

    auto size = image->getSize();
    u32 height = npot2(size.Y);
    u32 width  = npot2(size.X);

    if (size.Y == height && size.X == width)
        return image;

    if (size.Y > height)
        height *= 2;
    if (size.X > width)
        width *= 2;

    img::Image *targetimage = new Image(img::PF_RGBA8, width, height);
    mdf->copyTo(image, targetimage, nullptr, nullptr, true);
    delete image;

    return targetimage;
}

img::Image *applyCleanScalePowerOf2(img::Image *src, const v2i &src_size,
    const v2i &dest_size, img::ImageModifier *mdf)
{
    img::Image *img_copy = src->copy();
    imageCleanTransparent(img_copy, 0, mdf);

    v2u scaled_size(
        npot2(dest_size.X),
        npot2(dest_size.Y)
    );

    if (dest_size.Y > scaled_size.Y)
        scaled_size.Y *= 2;
    if (dest_size.X > scaled_size.X)
        scaled_size.X *= 2;
    img::Image *img_scaled = new img::Image(src->getFormat(), scaled_size.X, scaled_size.Y);
    imageScaleNNAA(img_copy, recti(src_size), img_scaled, mdf);

    return img_scaled;
}

}
