#include "StreamTexture2D.h"
#include "Common.h"
#include "Image/Converting.h"

namespace render
{

StreamTexture2D::StreamTexture2D(const std::string &name, u32 width, u32 height,
    img::PixelFormat format, const TextureSettings &settings)
    : Texture2D(name, std::make_unique<img::Image>(format, width, height), settings)
{
    createNewPBO(width, height);
}

StreamTexture2D::StreamTexture2D(const std::string &name, img::Image *img,
    const TextureSettings &settings)
    : Texture2D(name, std::unique_ptr<img::Image>(img), settings)
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

void StreamTexture2D::resize(u32 newWidth, u32 newHeight, img::ImageModifier *imgMod)
{
    u32 oldWidth = imgCache->getWidth();
    u32 oldHeight = imgCache->getHeight();

    if (newWidth <= oldWidth || newHeight <= oldHeight) // only increasing is allowed
        return;

    img::Image *newImgCache = new img::Image(format, newWidth, newHeight);
    rectu dstRect(0, 0, imgCache->getWidth(), imgCache->getHeight());
    imgMod->copyTo(imgCache.get(), newImgCache, nullptr, &dstRect);
    imgCache.reset(newImgCache);

    Texture2D::bind();

    auto &formatInfo = img::pixelFormatInfo.at(format);
    glTexImage2D(GL_TEXTURE_2D, 0, formatInfo.internalFormat, newWidth, newHeight, 0,
        formatInfo.pixelFormat, formatInfo.pixelType, imgCache->getData());
    TEST_GL_ERROR();

    if (texSettings.hasMipMaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
        TEST_GL_ERROR();
    }

    Texture2D::unbind();

    createNewPBO(newWidth, newHeight);

    width = newWidth;
    height = newHeight;
}

void StreamTexture2D::uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod)
{
    if (img->getFormat() != format) {
        ErrorStream << "StreamTexture2D::uploadSubData() the format of the uploaded image data is different\n";
        return;
    }

    v2u pos = img->getClipPos();
    v2u size = img->getClipSize();

    if (imgMod) {
        rectu srcRect(pos, size.X, size.Y);
        rectu destRect(x, y, x+size.X, y+size.Y);
        imgMod->copyTo(img, imgCache.get(), &srcRect, &destRect);
    }

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
    
    TEST_GL_ERROR();

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

    pboData = (u8 *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    TEST_GL_ERROR();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    TEST_GL_ERROR();
}

void StreamTexture2D::deletePBO()
{
    if (pboID == 0)
        return;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
    TEST_GL_ERROR();

    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    TEST_GL_ERROR();
    pboData = nullptr;

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    glDeleteBuffers(1, &pboID);
    TEST_GL_ERROR();
}

}
