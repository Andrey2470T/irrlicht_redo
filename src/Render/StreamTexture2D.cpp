#include "StreamTexture2D.h"
#include "Common.h"
#include "Image/Converting.h"

namespace render
{

StreamTexture2D::StreamTexture2D(const std::string &name, u32 width, u32 height,
    img::PixelFormat format, u8 maxMipLevel)
    : Texture2D(name, width, height, format, 0, maxMipLevel)
{
    createNewPBO(width, height);
}

StreamTexture2D::~StreamTexture2D()
{
    deletePBO();
}

void StreamTexture2D::bind()
{
    if (bound)
        return;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
    TEST_GL_ERROR();

    Texture2D::bind();
}

void StreamTexture2D::unbind()
{
    if (!bound)
        return;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    TEST_GL_ERROR();

    Texture2D::unbind();
}

void StreamTexture2D::mapPBO()
{
    if (pboData || !bound)
        return;

    pboData = (u8 *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    TEST_GL_ERROR();
}

void StreamTexture2D::unmapPBO()
{
    if (!pboData || !bound)
        return;

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    TEST_GL_ERROR();
    pboData = nullptr;
}

void StreamTexture2D::uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod)
{
    v2u pos = img->getClipPos();
    v2u size = img->getClipSize();

    auto convImg = img::convertIndexImageToRGBA(img);

    auto pixelSize = img::pixelFormatInfo.at(format).size / 8;
    u8* srcData = convImg ? convImg->getData() : img->getData();
    u32 imgWidth = img->getWidth();

    for (u32 row = 0; row < size.Y; ++row) {
        memcpy(pboData + ((y + row) * width + x) * pixelSize,
               srcData + ((pos.Y + row) * imgWidth + pos.X) * pixelSize,
               size.X * pixelSize);
    }

    if (convImg)
        delete convImg;

    dirtyRegions.emplace_back(x, y, x+size.X, y+size.Y);
}

void StreamTexture2D::flush()
{
    if (dirtyRegions.empty())
        return;

    std::vector<rectu> mergedRegions;
    mergeRegions(mergedRegions);

    bind();

    auto formatInfo = img::pixelFormatInfo.at(format);
    auto pixelSize = formatInfo.size / 8;

    for (auto &region : mergedRegions) {
        u32 x = region.ULC.X;
        u32 y = region.ULC.Y;
        u32 w = region.getWidth();
        u32 h = region.getHeight();
        u32 offset = (y * width + x) * pixelSize;

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
            formatInfo.pixelFormat, formatInfo.pixelType, (void *)(uintptr_t)offset);
        TEST_GL_ERROR();
    }

    dirtyRegions.clear();

    unbind();
}

void StreamTexture2D::mergeRegions(std::vector<rectu> &mregions) const
{
    mregions.clear();
    if (dirtyRegions.empty())
        return;

    u32 atlasArea = width * height;

    u32 changedSumArea = 0;

    for (const auto &region : dirtyRegions)
        changedSumArea += region.getArea();

    if (changedSumArea / (f32)atlasArea < 0.25f) {
        mregions = dirtyRegions;
    }
    else {
        rectu mergedRect = dirtyRegions.at(0);

        for (const auto &region : dirtyRegions) {
            mergedRect.ULC.X = std::min(mergedRect.ULC.X, region.ULC.X);
            mergedRect.ULC.Y = std::min(mergedRect.ULC.Y, region.ULC.Y);
            mergedRect.LRC.X = std::max(mergedRect.LRC.X, region.LRC.X);
            mergedRect.LRC.Y = std::max(mergedRect.LRC.Y, region.LRC.Y);
        }

        mregions.push_back(mergedRect);
    }
}

void StreamTexture2D::createNewPBO(u32 newWidth, u32 newHeight)
{
    deletePBO();

    glGenBuffers(1, &pboID);
    TEST_GL_ERROR();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
    TEST_GL_ERROR();

    auto pixelSize = img::pixelFormatInfo.at(format).size / 8;
    glBufferData(GL_PIXEL_UNPACK_BUFFER, newWidth * newHeight * pixelSize, nullptr, GL_STREAM_DRAW);
    TEST_GL_ERROR();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    TEST_GL_ERROR();
}

void StreamTexture2D::deletePBO()
{
    if (pboID == 0)
        return;

    glDeleteBuffers(1, &pboID);
    TEST_GL_ERROR();
}

}
