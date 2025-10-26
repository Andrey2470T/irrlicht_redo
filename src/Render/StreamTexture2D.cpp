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
    if (fence)
        glDeleteSync(fence);

    deletePBO();
}

void StreamTexture2D::bind()
{
    Texture2D::bind();

    if (bound)
        return;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
    TEST_GL_ERROR();
    bound = true;
}

void StreamTexture2D::unbind()
{
    Texture2D::unbind();

    if (!bound)
        return;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    TEST_GL_ERROR();
    bound = false;
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
    glTexImage2D(GL_TEXTURE_2D, 0, formatInfo.internalFormat, newWidth, newHeight, 0, formatInfo.pixelFormat, formatInfo.pixelType, imgCache->getData());
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
    auto pixelSize = img::pixelFormatInfo.at(format).size / 8;

    if (imgMod) {
        rectu srcRect(pos.X, pos.Y, size.X, size.Y);
        rectu destRect(x, y, size.X, size.Y);
        imgMod->copyTo(img, imgCache.get(), &srcRect, &destRect);
    }

    auto convImg = img::convertIndexImageToRGBA(img);

    u8* srcData = convImg ? convImg->getData() : img->getData();
    for (u32 row = 0; row < size.Y; ++row) {
        memcpy(pboData + ((y + row) * width + x) * pixelSize,
               srcData + row * size.X * pixelSize,
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

    if (fence) {
        glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        TEST_GL_ERROR();
        glDeleteSync(fence);
        TEST_GL_ERROR();
    }

    std::vector<rectu> mergedRegions;
    mergeRegions(mergedRegions);

    bind();

    auto formatInfo = img::pixelFormatInfo.at(format);
    for (auto &region : dirtyRegions) {
        u32 x = region.ULC.X;
        u32 y = region.ULC.Y;
        u32 width = region.getWidth();
        u32 height = region.getHeight();
        u32 offset = (y * width + x) * formatInfo.size;
        u32 length = width * height * formatInfo.size;
        glFlushMappedBufferRange(GL_PIXEL_UNPACK_BUFFER, offset, length);
        TEST_GL_ERROR();

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, formatInfo.pixelFormat, formatInfo.pixelType, nullptr);
        TEST_GL_ERROR();
    }

    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
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

    if (changedSumArea / (f32)atlasArea < 0.25) {
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
    /*for (const auto &region : dirtyRegions) {
        auto rsize = region.getSize();

        for (auto &mregion : mregions) {
            auto mrsize = mregion.getSize();
            v2u dist = region.getCenter() - mregion.getCenter();

            if (dist.X == (rsize.X/2 + mrsize.X/2) &&
                dist.Y == (rsize.Y/2 + mrsize.Y/2)) {
                mregion.addInternalPoint(region.ULC);
                mregion.addInternalPoint(region.LRC);

                break;
            }
            else
                mregions.emplace_back(region);
        }
    }*/
}

void StreamTexture2D::createNewPBO(u32 newWidth, u32 newHeight)
{
    deletePBO();

    glGenBuffers(1, &pboID);
    TEST_GL_ERROR();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
    TEST_GL_ERROR();

    auto pixelSize = img::pixelFormatInfo.at(format).size / 8;
    glBufferStorage(GL_PIXEL_UNPACK_BUFFER, newWidth * newHeight * pixelSize, nullptr,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    TEST_GL_ERROR();

    pboData = (u8*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, newWidth * newHeight * pixelSize,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    TEST_GL_ERROR();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    TEST_GL_ERROR();
}

void StreamTexture2D::deletePBO()
{
    if (pboID == 0)
        return;

    bind();

    if (pboData) {
        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        TEST_GL_ERROR();

        pboData = nullptr;
    }

    unbind();

    glDeleteBuffers(1, &pboID);
    TEST_GL_ERROR();
}

}
